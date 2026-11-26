#!/usr/bin/env bash
#
# Make AllShowers available for running it through DDML (via the
# AS_BARREL_PY_INTERFACE and AS_ENDCAP_PY_INTERFACE presets).
#
# Source this script on top of an environment that provides DDML and a Key4hep
# stack providing torch (>= 2.13 for the sparse attention mask):
#
#   source setup_allshowers.sh [INSTALL_DIR]
#
# The python packages that are needed on top of that (AllShowers, PointCountFM
# and a few dependencies) are installed into INSTALL_DIR with uv, unless they
# are already present there. INSTALL_DIR is put on the PYTHONPATH.
#
# INSTALL_DIR defaults to ${ALLSHOWERS_INSTALL_DIR}, or if that is not set to
# allshowers_libs in the current directory.
#
# Further environment variables that are considered:
#   PYTHON            Python interpreter to install for (default: python3)
#   ALLSHOWERS_REF    AllShowers git ref to install
#   POINTCOUNTFM_REF  PointCountFM git ref to install
#   UV_CACHE_DIR      uv cache location (e.g. if the default disk is small)

_allshowers_setup() {
  local install_dir=${1:-${ALLSHOWERS_INSTALL_DIR:-${PWD}/allshowers_libs}}
  local allshowers_ref=${ALLSHOWERS_REF:-05a458a1b7de0cb0e217266cecc13d42942c1c37}
  local pointcountfm_ref=${POINTCOUNTFM_REF:-a0a11c617ae79680a5426cb55afec18a1a32fb6e}

  # Package directories that are needed on top of the Key4hep stack and the
  # package that provides them
  local -A packages=(
    [allshowers]="git+https://github.com/FLC-QU-hep/AllShowers@${allshowers_ref}"
    [pointcountfm]="git+https://github.com/FLC-QU-hep/PointCountFM@${pointcountfm_ref}"
    [showerdata]=showerdata
    [ot]=pot
    [rich]=rich
  )

  mkdir -p "${install_dir}" || return 1
  install_dir=$(cd "${install_dir}" && pwd -P) || return 1

  local pkg_dir
  local -a missing=()
  for pkg_dir in "${!packages[@]}"; do
    if [[ ! -d "${install_dir}/${pkg_dir}" ]]; then
      missing+=("${packages[${pkg_dir}]}")
    fi
  done

  if [[ ${#missing[@]} -gt 0 ]]; then
    # --no-deps, to make sure nothing from the environment (most importantly
    # torch) gets installed again
    uv pip install --python "$(command -v "${PYTHON:-python3}")" --target "${install_dir}" --no-deps "${missing[@]}" || return 1
  fi

  if [[ ":${PYTHONPATH:-}:" != *":${install_dir}:"* ]]; then
    export PYTHONPATH=${install_dir}${PYTHONPATH:+:${PYTHONPATH}}
  fi
}

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
  echo "ERROR: This script has to be sourced: source ${BASH_SOURCE[0]} [INSTALL_DIR]" >&2
  exit 1
fi

if ! _allshowers_setup "$@"; then
  unset -f _allshowers_setup
  return 1
fi
unset -f _allshowers_setup
