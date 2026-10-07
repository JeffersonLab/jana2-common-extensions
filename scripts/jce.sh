#!/usr/bin/env bash
#
# jce.sh:
# Bash counterpart to jce.csh. Wrapper around jana that ensures:
#   - evio_parser is always first in -Pplugins
#   - JCE plugin path is prepended to jana_plugin_path/JANA_PLUGIN_PATH

passthrough_args=()
input_plugins=""
cli_plugin_path=""
cli_default_plugins_file=""
core_config_dir=""

for arg in "$@"; do
    case "$arg" in
        -Pplugins=*)
            input_plugins="$arg"
            ;;
        -Pjana:plugin_path=*)
            cli_plugin_path="$arg"
            ;;
        -PJCE:CORE_CONFIG_DIR=*)
            core_config_dir="${arg#-PJCE:CORE_CONFIG_DIR=}"
            ;;
        -PDEFAULT_PLUGINS:FILE=*)
            cli_default_plugins_file="$arg"
            ;;
        *)
            passthrough_args+=("$arg")
            ;;
    esac
done

jce_root="${JCE_HOME:-}"
if [[ -z "$jce_root" ]]; then
    echo "jce.sh error: set JCE_HOME to the root of the JCE installation."
    exit 2
fi

jce_plugin_dir="${jce_root}/lib/plugins"

warn() {
    if [[ -t 2 ]]; then
        printf '\033[33m%s\033[0m\n' "jce.sh warning: $*" >&2
    else
        printf '%s\n' "jce.sh warning: $*" >&2
    fi
}

read_plugins() {
    awk '!/^[[:space:]]*#/ { gsub(/[[:space:]]/, ""); if (length) print }' "$1"
}

# Explicit file overrides only the core layer; directory additions still apply.
default_plugins_file="${core_config_dir:-${jce_root}/config}/default_plugins.db"
if [[ -n "$cli_default_plugins_file" ]]; then
    default_plugins_file="${cli_default_plugins_file#-PDEFAULT_PLUGINS:FILE=}"
fi
core_plugins=""
if [[ -f "$default_plugins_file" ]]; then
    core_plugins=$(read_plugins "$default_plugins_file") || exit 2
else
    warn "default plugins file not found: $default_plugins_file; using core fallback"
fi
if [[ -z "${core_plugins//,/}" ]]; then
    core_plugins="evio_parser,evio_common_modules,detector_translation"
fi

plugin_lists=("evio_parser" "$core_plugins")
if [[ -n "${JCE_CONFIG_DIR:-}" ]]; then
    IFS=: read -r -a config_dirs <<< "$JCE_CONFIG_DIR"
    for config_dir in "${config_dirs[@]}"; do
        [[ -z "$config_dir" ]] && continue
        if [[ ! -d "$config_dir" ]]; then
            warn "configuration directory not found: $config_dir; skipping"
            continue
        fi
        if [[ -f "$config_dir/default_plugins.db" ]]; then
            plugins=$(read_plugins "$config_dir/default_plugins.db") || exit 2
            plugin_lists+=("$plugins")
        fi
    done
fi
plugin_lists+=("${input_plugins#-Pplugins=}")
merged_plugins=$(printf '%s\n' "${plugin_lists[@]}" | tr ',' '\n' |
    awk '{ gsub(/[[:space:]]/, ""); if (length && !seen[$0]++) {
        printf "%s%s", separator, $0; separator=","
    }} END { print "" }') || exit 2

user_plugin_path=""
if [[ -z "$cli_plugin_path" ]]; then
    user_plugin_path="${JANA_PLUGIN_PATH:-}"
else
    user_plugin_path="$cli_plugin_path"
    if [[ "$user_plugin_path" == -Pjana:plugin_path=* ]]; then
        user_plugin_path="${user_plugin_path#-Pjana:plugin_path=}"
    else
        echo "jce.sh error: invalid plugin path: $user_plugin_path"
        exit 2
    fi
fi

final_plugin_path="$jce_plugin_dir"
if [[ -n "$user_plugin_path" ]]; then
    final_plugin_path="${final_plugin_path}:${user_plugin_path}"
fi

export JANA_PLUGIN_PATH="$final_plugin_path"

final_args=("${passthrough_args[@]}" "-Pplugins=${merged_plugins}" "-Pjana:plugin_path=${final_plugin_path}")

jana_cmd=""
if [[ -x "${jce_root}/bin/jana" ]]; then
    jana_cmd="${jce_root}/bin/jana"
elif [[ -n "${JANA_HOME:-}" && -x "${JANA_HOME}/bin/jana" ]]; then
    jana_cmd="${JANA_HOME}/bin/jana"
fi

if [[ -z "$jana_cmd" ]]; then
    if command -v jana >/dev/null 2>&1; then
        jana_cmd="jana"
    else
        echo "jce.sh error: jana not found."
        echo "Expected ${jce_root}/bin/jana from the superbuild."
        echo "For a split-prefix manual installation, set JANA_HOME or add jana to PATH."
        exit 2
    fi
fi

exec "$jana_cmd" "${final_args[@]}"
