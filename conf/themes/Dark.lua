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
  chrome           = '#1E1C1B', -- menu bar, toolbar and status bar
  sidebar          = '#181715', -- repository sidebar
  content          = '#252322', -- lists, diffs and other content panes
  subtle           = '#2D2B29', -- headers and gutters inside content panes
  hover            = '#33312F', -- hovered flat buttons
  divider          = '#3B3937', -- lines between panes
  control_border   = '#4B4845', -- outlines of input fields
  text             = '#DAD6D2',
  text_secondary   = '#B9B5B1',
  text_muted       = '#9F9B97',
  accent           = '#6A4DD7',
  accent_subtle    = '#464460', -- selected and toggled-on backgrounds
  accent_text      = '#B2ACFE', -- accent colored text and links
  on_accent        = '#FFFFFF', -- text on accent filled buttons
  hunk_header      = '#2D2C37', -- diff hunk header band
  tab              = '#181715', -- unselected repository tabs
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
  text             = { default = t.text, disabled = '#6E6A67' },
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
  text             = { default = t.text, disabled = '#6E6A67' },
  background       = { default = t.subtle, checked = t.accent_subtle, pressed = t.hover }
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
    normal         = '#FFFFFF',
    selected       = '#FFFFFF',
    head           = '#FFFFFF',
    conflicted     = '#FFFFFF',
    notification   = '#FFFFFF',
    modified       = '#E7B369',
    added          = '#56D3D7',
    deleted        = '#F6A97A',
    untracked      = '#B2ACFE',
    renamed        = '#8DB8FA'
  },
  background       = {
    normal         = '#2272D0', -- the default color
    selected       = '#2272D0', -- the color when a list item is selected
    conflicted     = '#B4232A', -- the color of conflicted items
    head           = '#3B7F0B', -- a bolder color to indicate the HEAD
    notification   = '#B4232A', -- the color of toolbar notifications badges
    modified       = '#4A330F', -- (yellow) the color of the badge when the file is modified
    added          = '#0F3A3B', -- (green) the color of the badge when the file was newly added
    deleted        = '#4A2410', -- (red) the color of the badge when the file was deleted
    untracked      = '#2F2A4A', -- (green blue) the color of the badge when the file is untracked
    renamed        = '#1E3352'  -- (blue) the color of the badge when the file is renamed
  }
}

-- blame margin heatmap background colors
theme['blame'] = {
  cold             = '#282940',
  hot              = '#5E3638'
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
  outline          = '#6E6A66',
  checked          = t.accent
}

-- commit editor colors
theme['commiteditor'] = {
  spellerror       = '#BC0009', -- spell check error
  spellignore      = '#E1E5F2', -- spell check ignored word(s)
  lengthwarning    = '#464614'  -- line length limit warning (background)
}

-- diff view colors
-- Added and removed differ in hue (teal vs orange) and lightness, so they stay
-- distinct with red-green colour blindness.
theme['diff']      = {
  addition         = '#023C3E', -- added lines
  deletion         = '#492104', -- deleted lines
  plus             = '#56D3D7', -- plus icon
  minus            = '#F6A97A', -- minus icon
  ours             = '#000060', -- ours conflict lines
  theirs           = '#600060', -- theirs conflict lines
  word_addition    = '#066264', -- added words
  word_deletion    = '#7B3A02', -- deleted words
  note             = '#E1E5F2', -- note squiggle
  warning          = '#E8C080', -- warning background
  error            = '#7E494B'  -- error background
}

-- link colors
-- { default, active, inactive, disabled }
theme['link']      = {
  link             = t.accent_text,
  link_visited     = '#FF00FF'
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
  body             = '#AAB2BE',
  author           = '#378BDD',
  timestamp        = '#E1E5F2'
}

-- star fill color
theme['star']      = {
  fill             = '#FFFFFF'
}

-- titlebar background color (currently only supported on macOS)
theme['titlebar']  = {
  background       = t.chrome
}

-- popup tooltip colors
-- { default, active, inactive, disabled }
theme['tooltip']   = {
  text             = '#E1E5F2',
  background       = '#2A82DA'
}

-- editor styles
-- Styles are composed of a string like:
--   fore:<color>,back:<color>,bold,italics,underline
-- Symbolic style names are allowed:
--   $(style.name)
-- http://www.scintilla.org/MyScintillaDoc.html#Styling

-- colors (readable on the diff line and word highlight colors as well)
theme.property['color.red']          = '#F2727A'
theme.property['color.yellow']       = '#F7C790'
theme.property['color.green']        = '#87E4DE'
theme.property['color.teal']         = '#87E4DE'
theme.property['color.purple']       = '#CDCBFF'
theme.property['color.orange']       = '#F7C790'
theme.property['color.blue']         = '#9DDAFE'
theme.property['color.pink']         = '#FDBADF'
theme.property['color.black']        = t.text
theme.property['color.grey']         = '#ADA9A5'
theme.property['color.white']        = t.content

-- styles
theme.property['style.bracebad']     = 'fore:$(color.red)'
theme.property['style.bracelight']   = 'fore:$(color.blue),bold'
theme.property['style.calltip']      = 'fore:' .. t.text_secondary .. ',back:' .. t.subtle
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
theme.property['style.indentguide']  = 'fore:' .. t.divider .. ',back:' .. t.divider
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
  background       = '#4A3B12',
  foreground       = '#FFE9A8'
}
