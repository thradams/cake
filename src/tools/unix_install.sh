#!/bin/sh
#
# Linux / macOS installer, copied as install.sh into cake-<version>-<os>.tar.gz
#
#   tar xzf cake-<version>-<os>.tar.gz
#   cd cake-<version>-<os>
#   sudo ./install.sh
#
# Installs in $INSTALL_PREFIX/cake/<version> (default /usr/local), versions
# side by side. The installed version is the one in the PATH (new shells).
# To remove: sudo /usr/local/cake/<version>/uninstall.sh

set -e
cd "$(dirname "$0")"

VERSION=$(cat VERSION)
DEST=${INSTALL_PREFIX:-/usr/local}/cake/$VERSION

if [ "$(uname)" = "Darwin" ]; then
    PATH_FILE=/etc/paths.d/cake          # read by path_helper (all shells)
    PATH_LINE=$DEST
else
    PATH_FILE=/etc/profile.d/cake.sh     # read by login shells
    PATH_LINE="export PATH=\"$DEST:\$PATH\""
fi

echo "Installing cake $VERSION in $DEST"
rm -rf "$DEST"
mkdir -p "$DEST"
cp -R cake cakeide cake.json samples web include "$DEST/"

echo "$PATH_LINE" > "$PATH_FILE"
echo "PATH updated ($PATH_FILE)."
echo "Open a new terminal, or use it now with:"
if [ "$(uname)" = "Darwin" ]; then
    echo "  export PATH=\"$DEST:\$PATH\""
else
    echo "  source $PATH_FILE"
fi

cat > "$DEST/uninstall.sh" <<EOF
#!/bin/sh
# removes cake $VERSION (sudo $DEST/uninstall.sh)
rm -rf "$DEST"
rmdir "$(dirname "$DEST")" 2>/dev/null || true
grep -q "$DEST" "$PATH_FILE" 2>/dev/null && rm -f "$PATH_FILE"
echo "cake $VERSION removed"
EOF
chmod +x "$DEST/uninstall.sh"

echo "Done. To remove: sudo $DEST/uninstall.sh"
