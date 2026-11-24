--
-- Many colors support 'active', 'inactive', and 'disabled' states.
-- They can all be set to the same color with the syntax:
--
--   key = '<color>'
--
-- Or set individually with syntax like:
--
--   key = { active = '<color>', inactive = '<color>', disabled = '<color>' }
--
-- Use the 'default' key set one state individually and the remainder
-- to a default value:
--
--   key = { default = '<color>', disabled = '<color>' }
--

-- design tokens, referenced by the application style sheet as @<name>
local t = {
  chrome           = '#F4F3F2', -- menu bar, toolbar and status bar
  sidebar          = '#EFEDEC', -- repository sidebar
  content          = '#FFFDFC', -- lists, diffs and other content panes
  subtle           = '#F9F7F6', -- headers and gutters inside content panes
  hover            = '#EBE9E7', -- hovered flat buttons
  divider          = '#DEDCDA', -- lines between panes
  control_border   = '#CFCBC7', -- outlines of input fields
  text             = '#36312D',
  text_secondary   = '#55504C',
  text_muted       = '#69645F',
  accent           = '#674AD3',
  accent_subtle    = '#E1E1F5', -- selected and toggled-on backgrounds
  accent_text      = '#5C40C0', -- accent colored text and links
  on_accent        = '#FFFFFF', -- text on accent filled buttons
  hunk_header      = '#F1F1FA', -- diff hunk header band
  tab              = '#E8E6E3', -- unselected repository tabs
}
theme['tokens'] = t

-- generic colors used to render borders and separators
-- { default, active, inactive, disabled }
theme['palette']   = {
  -- These names correspond to a dark on light theme.
  -- The values should be inverted in light on dark themes.
  light            = t.content,
  midlight         = t.subtle,
  middark          = t.control_border,
  dark             = t.divider,

  -- Used for widget borders and outlines.
  shadow           = t.control_border
}

-- the colors of text entry, list view, and other widgets
-- { default, active, inactive, disabled }
theme['widget']    = {
  text             = { default = t.text, disabled = '#A19D99' },
  bright_text      = t.text_secondary,
  background       = t.content,
  alternate        = t.subtle, -- an alternate background color for list rows
  highlight        = t.accent_subtle,
  highlighted_text = t.text,
}

-- window colors
-- { default, active, inactive, disabled }
theme['window']    = {
  text             = t.text,
  background       = t.chrome
}

-- button colors
-- { default, active, inactive, disabled, checked, pressed }
theme['button']    = {
  text             = { default = t.text, disabled = '#A19D99' },
  background       = { default = t.content, checked = t.accent_subtle, pressed = t.hover }
}

-- commit list colors
-- { default, active, inactive, disabled }
theme['commits']   = {
  text             = t.text,
  bright_text      = t.text,
  background       = t.content,
  alternate        = t.subtle, -- an alternate background color for list rows
  highlight        = t.accent_subtle,
  highlighted_text = t.text,
  highlighted_bright_text = t.text,
  muted_text       = t.text_muted -- commit id and date
}

-- status badge colors
-- { normal, selected, conflicted, head, notification }
theme['badge']     = {
  foreground       = {
    normal         = '#103869',
    selected       = '#103869',
    head           = '#1E4607',
    conflicted     = '#FFFFFF',
    notification   = '#FFFFFF',
    modified       = '#774F02',
    added          = '#016568',
    deleted        = '#894101',
    untracked      = '#4A3A99',
    renamed        = '#103869'
  },
  background       = {
    normal         = '#C3D9F7', -- the default color
    selected       = '#C3D9F7', -- the color when a list item is selected
    conflicted     = '#B4232A', -- the color of conflicted items
    head           = '#C6E8B3', -- a bolder color to indicate the HEAD
    notification   = '#B4232A', -- the color of toolbar notifications badges
    modified       = '#FDE8CB', -- (yellow) the color of the badge when the file is modified
    added          = '#D3F1F2', -- (green) the color of the badge when the file was newly added
    deleted        = '#FBE0D2', -- (red) the color of the badge when the file was deleted
    untracked      = '#E4E2F7', -- (green blue) the color of the badge when the file is untracked
    renamed        = '#D8E6FA'  -- (blue) the color of the badge when the file is renamed
  },
  -- a slightly deeper edge so the light badges stay defined on selected rows
  border           = {
    normal         = '#9FC0EC',
    selected       = '#9FC0EC',
    conflicted     = '#B4232A',
    head           = '#9CCF81',
    notification   = '#B4232A',
    modified       = '#FDE8CB',
    added          = '#D3F1F2',
    deleted        = '#FBE0D2',
    untracked      = '#E4E2F7',
    renamed        = '#D8E6FA'
  }
}

-- blame margin heatmap background colors
theme['blame'] = {
  cold             = '#C0C0FF',
  hot              = '#FFC0C0'
}

-- graph edge colors
theme['graph']     = {
  edge1            = '#53AFEC',
  edge2            = '#82DA2A',
  edge3            = '#DA2ADA',
  edge4            = '#DA822A',
  edge5            = '#2ADADA',
  edge6            = '#DA2A82',
  edge7            = '#84A896',
  edge8            = '#2ADA82',
  edge9            = '#822ADA',
  edge10           = '#66D1E0',
  edge11           = '#D3C27E',
  edge12           = '#95CB80',
  edge13           = '#50D4BE',
  edge14           = '#2ADA82',
  edge15           = '#DA822A'
}

-- checkbox colors
-- { default, active, inactive, disabled }
theme['checkbox']  = {
  text             = '#FFFFFF', -- check mark
  fill             = t.content,
  outline          = '#B5B0AB',
  checked          = t.accent
}

-- commit editor colors
theme['commiteditor'] = {
  spellerror       = '#FF0000', -- spell check error
  spellignore      = '#A0A0A4', -- spell check ignored word(s)
  lengthwarning    = '#EFF0F1'  -- line length limit warning (background)
}

-- diff view colors
-- Added and removed differ in hue (teal vs orange) and lightness, so they stay
-- distinct with red-green colour blindness.
theme['diff']      = {
  addition         = '#B1F6F8', -- added lines
  deletion         = '#FFD2B7', -- deleted lines
  plus             = '#016568', -- plus icon
  minus            = '#894101', -- minus icon
  ours             = '#DCFFFF', -- ours conflict lines
  theirs           = '#FFDCFF', -- theirs conflict lines
  word_addition    = '#4DD4D8', -- added words
  word_deletion    = '#EF9C68', -- deleted words
  note             = '#000000', -- note squiggle
  warning          = '#FFFF00', -- warning background
  error            = '#FF0000'  -- error background
}

-- link colors
-- { default, active, inactive, disabled }
theme['link']      = {
  link             = t.accent_text,
  link_visited     = '#7F8C8D'
}

-- menubar background color
theme['menubar']   = {
  text             = t.text,
  background       = t.chrome
}

-- tabbar background color
theme['tabbar']   = {
  text             = t.text_secondary,
  base             = t.tab,
  selected         = t.chrome, -- flows into the repository view below
}

-- remote comment colors
theme['comment']   = {
  background       = t.content,
  body             = '#C2E0F5',
  author           = '#3DAEE9',
  timestamp        = '#232627'
}

-- star fill color
theme['star']      = {
  fill             = '#FFCE6D'
}

-- titlebar background color (currently only supported on macOS)
theme['titlebar']  = {
  background       = t.chrome
}

-- popup tooltip colors
-- { default, active, inactive, disabled }
theme['tooltip']   = {
  text             = '#FCFCFC',
  background       = '#232627'
}

-- editor styles
-- Styles are composed of a string like:
--   fore:<color>,back:<color>,bold,italics,underline
-- Symbolic style names are allowed:
--   $(style.name)
-- http://www.scintilla.org/MyScintillaDoc.html#Styling

-- colors (readable on the diff line and word highlight colors as well)
theme.property['color.red']          = '#B4232A'
theme.property['color.yellow']       = '#5A3700'
theme.property['color.green']        = '#004A47'
theme.property['color.teal']         = '#004A47'
theme.property['color.purple']       = '#3E2F7F'
theme.property['color.orange']       = '#5A3700'
theme.property['color.blue']         = '#014563'
theme.property['color.pink']         = '#6B184F'
theme.property['color.black']        = t.text
theme.property['color.grey']         = '#625D59'
theme.property['color.white']        = t.content

-- styles
theme.property['style.bracebad']     = 'fore:$(color.red)'
theme.property['style.bracelight']   = 'fore:$(color.blue),bold'
theme.property['style.calltip']      = '$(style.nothing)'
theme.property['style.class']        = 'fore:$(color.teal)'
theme.property['style.comment']      = 'fore:$(color.grey)'
theme.property['style.constant']     = 'fore:$(color.pink)'
theme.property['style.controlchar']  = '$(style.nothing)'
theme.property['style.default']      = 'fore:$(color.black),back:$(color.white)'
theme.property['style.definition']   = 'fore:$(color.blue)'
theme.property['style.embedded']     = '$(style.nothing)'
theme.property['style.error']        = 'fore:$(color.red)'
theme.property['style.function']     = 'fore:$(color.blue)'
theme.property['style.identifier']   = '$(style.nothing)'
theme.property['style.indentguide']  = '$(style.nothing)'
theme.property['style.keyword']      = 'fore:$(color.purple)'
theme.property['style.label']        = 'fore:$(color.purple)'
theme.property['style.linenumber']   = 'fore:' .. t.text_muted .. ',back:' .. t.subtle
theme.property['style.nothing']      = ''
theme.property['style.number']       = 'fore:$(color.pink)'
theme.property['style.operator']     = '$(style.nothing)'
theme.property['style.preprocessor'] = 'fore:$(color.purple)'
theme.property['style.regex']        = 'fore:$(color.orange)'
theme.property['style.string']       = 'fore:$(color.orange)'
theme.property['style.tag']          = 'fore:$(color.purple)'
theme.property['style.type']         = 'fore:$(color.teal)'
theme.property['style.variable']     = '$(style.nothing)'
theme.property['style.whitespace']   = '$(style.nothing)'
-- inline notice/banner colors (e.g. the "diff not loaded" placeholder)
theme['notice']    = {
  background       = '#FFF3CD',
  foreground       = '#664D03'
}
