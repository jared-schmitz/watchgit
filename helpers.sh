# =============================================================================
#  watchgit: Helper scripts.
#
#  This file is subject to the terms and conditions defined in
#  file 'LICENSE', which is part of this source code package.
#
#  Source this from your shell's rc.
# =============================================================================

# Changes directory to repository location.
# Reaps for directories using `watchgit list`.
function snap() {
  place=`watchgit list | sed -n "s/^${1}: //p"`

  if [ -z "${place}" ]; then
    echo 'watchgit: Unknown repository.'
  else
    cd "${place}"
  fi
}

