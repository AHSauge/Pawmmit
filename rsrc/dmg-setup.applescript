-- Finder layout for the macOS drag-and-drop installer window.
--
-- CPack's DragNDrop generator runs this script (via osascript) against the
-- mounted, writable DMG and captures the resulting .DS_Store. It replaces the
-- old pre-baked rsrc/DS_Store so the layout is reviewable in source control.
--
-- Geometry matches rsrc/dmg-background.png (660x400): the app bundle sits on
-- the left, the /Applications alias on the right, and the claw-swipe in the
-- artwork points from one to the other. Regenerate the background from
-- rsrc/dmg-background.svg if these positions change.
--
-- Note: the bundle is installed as "pawmmit.app" (lowercase), because the
-- executable/target name is "pawmmit". Update the item name below if that
-- ever changes.

tell application "Finder"
  tell disk "Pawmmit"
    open

    set current view of container window to icon view
    set toolbar visible of container window to false
    set statusbar visible of container window to false
    set the bounds of container window to {400, 100, 1060, 500}

    set theViewOptions to the icon view options of container window
    set arrangement of theViewOptions to not arranged
    set icon size of theViewOptions to 128
    set background picture of theViewOptions to file ".background:background.png"

    set position of item "pawmmit.app" of container window to {180, 190}
    set position of item "Applications" of container window to {480, 190}

    close
    open
    update without registering applications
    delay 5
  end tell
end tell
