/* ide_themes.c - the IDE's five themes: Ambar, Dark, White, Nebula and
 * Xcode Dark, copied from the old IDE (ide.c) unchanged - struct gui_theme
 * has the same fields as its ui_theme.
 */
#include "ide_shell.h"

/* Visual Studio Dark theme palette (editor #1E1E1E/#D4D4D4, chrome #2D2D30,
 * accent #007ACC). */
/* "Ambar" - dark chrome with an amber accent (#F5C242): the accent
 * carries buttons, the active border, selection and keywords, with a
 * warm grey text ramp and a gruvbox-leaning syntax palette. Replaces
 * the old "Classic" entry, which was only ever a snapshot of the
 * framework default. */
const struct gui_theme ide_theme_ambar = {
    /* NOTE: anything drawn ON the #F5C242 accent uses dark ink, not white -
     * white scores 1.66:1 against this amber (unreadable), dark ink 10:1.
     * Buttons are the exception: their text is light, so their hover state
     * stays grey instead of turning amber. */
    .desktop_bg = GUI_RGB(0x1E, 0x1E, 0x20),

    .btn_bg = GUI_RGB(0x3A, 0x3A, 0x3F),
    .btn_bg_hot = GUI_RGB(0x4A, 0x4A, 0x50),
    .btn_bg_active = GUI_RGB(0x7A, 0x5A, 0x12),
    .btn_fg = GUI_RGB(0xE8, 0xE4, 0xDA),

    .hotkey_fg = GUI_RGB(0x1E, 0x1E, 0x20),
    .hotkey_key_fg = GUI_RGB(0x8A, 0x3B, 0x0A),
    .hotkey_bg = GUI_RGB(0xF5, 0xC2, 0x42),
    .hotkey_fg_hot = GUI_RGB(0x1E, 0x1E, 0x20),
    .hotkey_bg_hot = GUI_RGB(0xFF, 0xD1, 0x66),

    .menu_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .menu_bg = GUI_RGB(0x2F, 0x2F, 0x35),
    .menu_fg_sel = GUI_RGB(0x1E, 0x1E, 0x20),
    .menu_bg_sel = GUI_RGB(0xF5, 0xC2, 0x42),
    .menu_item_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .menu_item_bg = GUI_RGB(0x26, 0x26, 0x2B),
    .menu_item_fg_hot = GUI_RGB(0x1E, 0x1E, 0x20),
    .menu_item_bg_hot = GUI_RGB(0xF5, 0xC2, 0x42),
    .menu_item_shortcut_fg = GUI_RGB(0xA8, 0xA2, 0x94),
    .menu_item_fg_disabled = GUI_RGB(0x65, 0x65, 0x65),
    .menu_border_fg = GUI_RGB(0x3A, 0x3A, 0x3F),  /* dark gray, not bright white */
    .menu_border_bg = GUI_RGB(0x26, 0x26, 0x2B),
    .menu_border_style = GUI_BORDER_SINGLE,

    .box_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .box_bg = GUI_RGB(0x26, 0x26, 0x2B),
    .box_border_style = GUI_BORDER_DOUBLE,

    .window_border_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .window_border_bg = GUI_RGB(0x2F, 0x2F, 0x35),
    .window_border_fg_dragging = GUI_RGB(0xF5, 0xC2, 0x42),
    .window_border_fg_unfocused = GUI_RGB(0x65, 0x65, 0x65),
    .window_border_style = GUI_BORDER_DOUBLE,
    .window_border_style_unfocused = GUI_BORDER_SINGLE,
    .window_border_style_docked = GUI_BORDER_SINGLE,  /* Output/Folder panels -
                                                       * a lighter frame than
                                                       * a floating document
                                                       * window's */
    .window_border_style_docked_unfocused = GUI_BORDER_SINGLE,
    .window_close_bg = GUI_RGB(0xE0, 0x3E, 0x36),
    .window_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .window_bg = GUI_RGB(0x26, 0x26, 0x2B),  /* VS's actual docked-panel
                                             * (Git Changes/Folder/Solution
                                             * Explorer) body color */
    .modal_border_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .modal_border_bg = GUI_RGB(0x26, 0x26, 0x2B),
    .modal_border_style = GUI_BORDER_DOUBLE,
    .modal_fg = GUI_RGB(0xE8, 0xE4, 0xDA),
    .modal_bg = GUI_RGB(0x26, 0x26, 0x2B),  /* VS's actual Find/Replace and
                                            * other dialog body color */
    .label_fg = GUI_RGB(0xF5, 0xC2, 0x42),  /* the theme's amber accent */
    .scrollbar_bg = GUI_RGB(0x2F, 0x2F, 0x35),  /* matches the window border */
    .scrollbar_thumb_bg = GUI_RGB(0x42, 0x42, 0x42),
    .scrollbar_thumb_hot_bg = GUI_RGB(0x6E, 0x6E, 0x6E),

    .input_bg = GUI_RGB(0x3C, 0x3C, 0x3C),  /* VS's actual text box/dropdown
                                            * fill - lighter than modal_bg
                                            * so fields stand out in dialogs */
    .input_bg_focus = GUI_RGB(0x1E, 0x1E, 0x20),  /* darker than the neutral
                                                  * gray input_bg, so focus is
                                                  * still visible - deliberately
                                                  * NOT a blue tint, since
                                                  * input_sel_bg below (the
                                                  * caret/selection block,
                                                  * #264F78) is a similar blue
                                                  * and would nearly vanish
                                                  * against a same-hued focus
                                                  * fill */
    .input_fg = GUI_RGB(0xD8, 0xD6, 0xD0),
    .input_fg_focus = GUI_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = GUI_RGB(0x5A, 0x48, 0x1C),
    .input_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = GUI_RGB(0x1E, 0x1E, 0x20),
    .editor_fg = GUI_RGB(0xD8, 0xD6, 0xD0),
    .editor_keyword_fg = GUI_RGB(0xF5, 0xC2, 0x42),   /* types/storage: VS blue */
    .editor_keyword2_fg = GUI_RGB(0xD3, 0x86, 0x9B),  /* control flow: VS purple */
    .editor_string_fg = GUI_RGB(0xB8, 0xBB, 0x6B),
    .editor_comment_fg = GUI_RGB(0x7C, 0x7C, 0x74),
    .editor_lint_fg = GUI_RGB(0xF5, 0xC2, 0x42),  /* same gold as
                                                  * editor_keyword_fg - stands
                                                  * out from the dim gray
                                                  * editor_comment_fg */
    .editor_linenum_fg = GUI_RGB(0x85, 0x85, 0x85),  /* VS Code Dark's actual
                                                     * gutter gray */
    .editor_preproc_fg = GUI_RGB(0xD3, 0x86, 0x9B),
    .editor_attribute_fg = GUI_RGB(0x8E, 0xC0, 0x7C),
    .editor_sel_bg = GUI_RGB(0x5A, 0x48, 0x1C),  /* #264F78 - VS Dark's actual selection color */
    .editor_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = GUI_RGB(0x3A, 0x33, 0x22),  /* a warm step up from
                                                        * editor_bg, matching
                                                        * the amber accent */
    .editor_current_line_bg = GUI_RGB(0x2A, 0x2A, 0x2A),  /* subtle - close to
                                                          * VS Code Dark's own
                                                          * current-line tint */
    .editor_breakpoint_fg = GUI_RGB(0xF4, 0x47, 0x47),  /* VS Code's own
                                                        * breakpoint red */
    .editor_exec_line_bg = GUI_RGB(0x1F, 0x3A, 0x1F),   /* subtle green step up
                                                        * from editor_bg */
    .editor_bracket_fg = {
        GUI_RGB(0xFF, 0xD1, 0x66),  /* gold */
        GUI_RGB(0xFF, 0xB4, 0x5A),  /* pink */
        GUI_RGB(0x40, 0xE0, 0xD0),  /* turquoise */
        GUI_RGB(0x8A, 0xB4, 0xF8),  /* steel blue */
    },
    .editor_tag_fg = GUI_RGB(0xE7, 0x8A, 0x4E),  /* teal - same accent as this
                                                 * theme's own hotkey_key_fg,
                                                 * VS Code Dark's actual type-
                                                 * name color */
    .editor_number_fg = GUI_RGB(0xB5, 0xCE, 0xA8),  /* pale green - VS Code
                                                     * Dark's actual numeric
                                                     * literal color */
    .editor_char_fg = GUI_RGB(0xF0, 0x55, 0x4E),  /* rose - Atom One Dark's
                                                   * character/constant color,
                                                   * distinct from string_fg's
                                                   * tan */
    .editor_function_fg = GUI_RGB(0xDC, 0xDC, 0xAA),  /* soft yellow - VS Code
                                                       * Dark's actual function-
                                                       * name color */
    .editor_output_bg = GUI_RGB(0x18, 0x18, 0x18),    /* one step darker than
                                                       * editor_bg (#1E1E1E) -
                                                       * VS Code Dark's recessed
                                                       * panel look for Output */
    .editor_output_fg = GUI_RGB(0xD8, 0xD6, 0xD0),    /* same as editor_fg - the
                                                       * dark panel keeps the
                                                       * plain light default */

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same VS Code Dark palette. */
    .md_heading_fg = GUI_RGB(0xF5, 0xC2, 0x42),     /* same VS blue as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = GUI_RGB(0x7C, 0x7C, 0x74),  /* same green as
                                                     * editor_comment_fg */
    .md_code_fg = GUI_RGB(0xB8, 0xBB, 0x6B),        /* same tan as
                                                     * editor_string_fg */
    .md_bold_fg = GUI_RGB(0xD8, 0xD6, 0xD0),        /* same as editor_fg */
    .md_link_fg = GUI_RGB(0xE9, 0xB4, 0x6A),        /* VS Code Dark's actual
                                                     * hyperlink blue */
    .editor_diff_add_bg = GUI_RGB(0x16, 0x3A, 0x2E),
    .editor_diff_remove_bg = GUI_RGB(0x3A, 0x1D, 0x1D),
    .md_code_bg = GUI_RGB(0x28, 0x2C, 0x34),        /* slate - subtly lighter/
                                                     * cooler than editor_bg
                                                     * and distinct from
                                                     * editor_current_line_bg,
                                                     * GitHub dark's own code-
                                                     * block tint */

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #007ACC accent as
     * every other "selected" state in this theme. */
    .listbox_fg = GUI_RGB(0x96, 0x94, 0x8E),  /* deliberately dimmer than
                                              * editor_fg: the Folder panel
                                              * is the biggest list on
                                              * screen and shouldn't pull
                                              * attention off the code */
    .listbox_bg = GUI_RGB(0x1E, 0x1E, 0x20),
    .listbox_sel_fg = GUI_RGB(0x1E, 0x1E, 0x20),
    .listbox_sel_bg = GUI_RGB(0xF5, 0xC2, 0x42),
    .listbox_sel_inactive_fg = GUI_RGB(0xD8, 0xD6, 0xD0),
    .listbox_sel_inactive_bg = GUI_RGB(0x3A, 0x3A, 0x3F),  /* VS's own muted gray */

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = GUI_RGB(0x56, 0x9C, 0xD6),   /* VS blue */
    .project_icon_h_fg = GUI_RGB(0xB5, 0xCE, 0xA8),   /* muted green */
    .project_icon_md_fg = GUI_RGB(0xE9, 0xB4, 0x6A),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - VS Code Dark's actual error/warning/info
     * squiggle colors, so they read as authentically part of this theme. */
    .diag_error_fg = GUI_RGB(0xF4, 0x47, 0x47),
    .diag_warning_fg = GUI_RGB(0xCC, 0xA7, 0x00),
    .diag_info_fg = GUI_RGB(0x37, 0x94, 0xFF),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = GUI_RGB(0x45, 0x25, 0x27),
    .diag_warning_bg = GUI_RGB(0x3D, 0x37, 0x1A),
    .diag_info_bg = GUI_RGB(0x22, 0x33, 0x48),
};

const struct gui_theme ide_theme_dark = {
    .desktop_bg = GUI_RGB(0x1E, 0x1E, 0x1E),

    .btn_bg = GUI_RGB(0x3F, 0x3F, 0x46),
    .btn_bg_hot = GUI_RGB(0x00, 0x7A, 0xCC),
    .btn_bg_active = GUI_RGB(0x00, 0x5A, 0x9E),
    .btn_fg = GUI_RGB(0xF1, 0xF1, 0xF1),

    .hotkey_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .hotkey_key_fg = GUI_RGB(0x4E, 0xC9, 0xB0),
    .hotkey_bg = GUI_RGB(0x00, 0x7A, 0xCC),
    .hotkey_fg_hot = GUI_RGB(0xFF, 0xFF, 0xFF),
    .hotkey_bg_hot = GUI_RGB(0x1C, 0x97, 0xEA),

    .menu_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .menu_bg = GUI_RGB(0x2D, 0x2D, 0x30),
    .menu_fg_sel = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_bg_sel = GUI_RGB(0x00, 0x7A, 0xCC),
    .menu_item_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .menu_item_bg = GUI_RGB(0x25, 0x25, 0x26),
    .menu_item_fg_hot = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_bg_hot = GUI_RGB(0x00, 0x7A, 0xCC),
    .menu_item_shortcut_fg = GUI_RGB(0x9C, 0xDC, 0xFE),
    .menu_item_fg_disabled = GUI_RGB(0x65, 0x65, 0x65),
    .menu_border_fg = GUI_RGB(0x3F, 0x3F, 0x46),  /* dark gray, not bright white */
    .menu_border_bg = GUI_RGB(0x25, 0x25, 0x26),
    .menu_border_style = GUI_BORDER_SINGLE,

    .box_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .box_bg = GUI_RGB(0x25, 0x25, 0x26),
    .box_border_style = GUI_BORDER_DOUBLE,

    .window_border_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .window_border_bg = GUI_RGB(0x2D, 0x2D, 0x30),
    .window_border_fg_dragging = GUI_RGB(0x00, 0x7A, 0xCC),
    .window_border_fg_unfocused = GUI_RGB(0x65, 0x65, 0x65),
    .window_border_style = GUI_BORDER_DOUBLE,
    .window_border_style_unfocused = GUI_BORDER_SINGLE,
    .window_border_style_docked = GUI_BORDER_SINGLE,  /* Output/Folder panels -
                                                       * a lighter frame than
                                                       * a floating document
                                                       * window's */
    .window_border_style_docked_unfocused = GUI_BORDER_SINGLE,
    .window_close_bg = GUI_RGB(0xE8, 0x11, 0x23),
    .window_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .window_bg = GUI_RGB(0x25, 0x25, 0x26),  /* VS's actual docked-panel
                                             * (Git Changes/Folder/Solution
                                             * Explorer) body color */
    .modal_border_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .modal_border_bg = GUI_RGB(0x25, 0x25, 0x26),
    .modal_border_style = GUI_BORDER_DOUBLE,
    .modal_fg = GUI_RGB(0xF1, 0xF1, 0xF1),
    .modal_bg = GUI_RGB(0x25, 0x25, 0x26),  /* VS's actual Find/Replace and
                                            * other dialog body color */
    .label_fg = GUI_RGB(0xFF, 0xFF, 0x55),
    .scrollbar_bg = GUI_RGB(0x2D, 0x2D, 0x30),  /* matches the window border */
    .scrollbar_thumb_bg = GUI_RGB(0x42, 0x42, 0x42),
    .scrollbar_thumb_hot_bg = GUI_RGB(0x6E, 0x6E, 0x6E),

    .input_bg = GUI_RGB(0x3C, 0x3C, 0x3C),  /* VS's actual text box/dropdown
                                            * fill - lighter than modal_bg
                                            * so fields stand out in dialogs */
    .input_bg_focus = GUI_RGB(0x1E, 0x1E, 0x1E),  /* darker than the neutral
                                                  * gray input_bg, so focus is
                                                  * still visible - deliberately
                                                  * NOT a blue tint, since
                                                  * input_sel_bg below (the
                                                  * caret/selection block,
                                                  * #264F78) is a similar blue
                                                  * and would nearly vanish
                                                  * against a same-hued focus
                                                  * fill */
    .input_fg = GUI_RGB(0xD4, 0xD4, 0xD4),
    .input_fg_focus = GUI_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = GUI_RGB(0x26, 0x4F, 0x78),
    .input_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .editor_fg = GUI_RGB(0xD4, 0xD4, 0xD4),
    .editor_keyword_fg = GUI_RGB(0x56, 0x9C, 0xD6),   /* types/storage: VS blue */
    .editor_keyword2_fg = GUI_RGB(0xC5, 0x86, 0xC0),  /* control flow: VS purple */
    .editor_string_fg = GUI_RGB(0xCE, 0x91, 0x78),
    .editor_comment_fg = GUI_RGB(0x6A, 0x99, 0x55),
    .editor_lint_fg = GUI_RGB(0x56, 0x9C, 0xD6),  /* same blue as
                                                  * editor_keyword_fg - stands
                                                  * out from the green
                                                  * editor_comment_fg */
    .editor_linenum_fg = GUI_RGB(0x85, 0x85, 0x85),  /* VS Code Dark's actual
                                                     * gutter gray */
    .editor_preproc_fg = GUI_RGB(0xC5, 0x86, 0xC0),
    .editor_attribute_fg = GUI_RGB(0x4E, 0xC9, 0xB0),
    .editor_sel_bg = GUI_RGB(0x26, 0x4F, 0x78),  /* #264F78 - VS Dark's actual selection color */
    .editor_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = GUI_RGB(0x33, 0x3A, 0x40),
    .editor_current_line_bg = GUI_RGB(0x2A, 0x2A, 0x2A),  /* subtle - close to
                                                          * VS Code Dark's own
                                                          * current-line tint */
    .editor_breakpoint_fg = GUI_RGB(0xF4, 0x47, 0x47),  /* VS Code's own
                                                        * breakpoint red */
    .editor_exec_line_bg = GUI_RGB(0x1F, 0x3A, 0x1F),   /* subtle green step up
                                                        * from editor_bg */
    .editor_bracket_fg = {
        GUI_RGB(0xFF, 0xD7, 0x00),  /* gold */
        GUI_RGB(0xFF, 0x6A, 0xC1),  /* pink */
        GUI_RGB(0x40, 0xE0, 0xD0),  /* turquoise */
        GUI_RGB(0x8A, 0xB4, 0xF8),  /* steel blue */
    },
    .editor_tag_fg = GUI_RGB(0x4E, 0xC9, 0xB0),  /* teal - same accent as this
                                                 * theme's own hotkey_key_fg,
                                                 * VS Code Dark's actual type-
                                                 * name color */
    .editor_number_fg = GUI_RGB(0xB5, 0xCE, 0xA8),  /* pale green - VS Code
                                                     * Dark's actual numeric
                                                     * literal color */
    .editor_char_fg = GUI_RGB(0xE0, 0x6C, 0x75),  /* rose - Atom One Dark's
                                                   * character/constant color,
                                                   * distinct from string_fg's
                                                   * tan */
    .editor_function_fg = GUI_RGB(0xDC, 0xDC, 0xAA),  /* soft yellow - VS Code
                                                       * Dark's actual function-
                                                       * name color */
    .editor_output_bg = GUI_RGB(0x18, 0x18, 0x18),    /* one step darker than
                                                       * editor_bg (#1E1E1E) -
                                                       * VS Code Dark's recessed
                                                       * panel look for Output */
    .editor_output_fg = GUI_RGB(0xD4, 0xD4, 0xD4),    /* same as editor_fg - the
                                                       * dark panel keeps the
                                                       * plain light default */

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same VS Code Dark palette. */
    .md_heading_fg = GUI_RGB(0x56, 0x9C, 0xD6),     /* same VS blue as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = GUI_RGB(0x6A, 0x99, 0x55),  /* same green as
                                                     * editor_comment_fg */
    .md_code_fg = GUI_RGB(0xCE, 0x91, 0x78),        /* same tan as
                                                     * editor_string_fg */
    .md_bold_fg = GUI_RGB(0xD4, 0xD4, 0xD4),        /* same as editor_fg */
    .md_link_fg = GUI_RGB(0x3D, 0xA8, 0xF5),        /* VS Code Dark's actual
                                                     * hyperlink blue */
    .editor_diff_add_bg = GUI_RGB(0x16, 0x3A, 0x2E),
    .editor_diff_remove_bg = GUI_RGB(0x3A, 0x1D, 0x1D),
    .md_code_bg = GUI_RGB(0x28, 0x2C, 0x34),        /* slate - subtly lighter/
                                                     * cooler than editor_bg
                                                     * and distinct from
                                                     * editor_current_line_bg,
                                                     * GitHub dark's own code-
                                                     * block tint */

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #007ACC accent as
     * every other "selected" state in this theme. */
    .listbox_fg = GUI_RGB(0x93, 0x93, 0x93),  /* dimmer than editor_fg - see
                                              * the Ambar theme's own note */
    .listbox_bg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .listbox_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_bg = GUI_RGB(0x00, 0x7A, 0xCC),
    .listbox_sel_inactive_fg = GUI_RGB(0xD4, 0xD4, 0xD4),
    .listbox_sel_inactive_bg = GUI_RGB(0x3F, 0x3F, 0x46),  /* VS's own muted gray */

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = GUI_RGB(0x56, 0x9C, 0xD6),   /* VS blue, matches this
                                                       * theme's own
                                                       * editor_keyword_fg */
    .project_icon_h_fg = GUI_RGB(0xB5, 0xCE, 0xA8),   /* muted green */
    .project_icon_md_fg = GUI_RGB(0x3D, 0xA8, 0xF5),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - VS Code Dark's actual error/warning/info
     * squiggle colors, so they read as authentically part of this theme. */
    .diag_error_fg = GUI_RGB(0xF4, 0x47, 0x47),
    .diag_warning_fg = GUI_RGB(0xCC, 0xA7, 0x00),
    .diag_info_fg = GUI_RGB(0x37, 0x94, 0xFF),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = GUI_RGB(0x45, 0x25, 0x25),
    .diag_warning_bg = GUI_RGB(0x3D, 0x37, 0x19),
    .diag_info_bg = GUI_RGB(0x22, 0x33, 0x46),
};

/* Visual Studio Light theme palette (editor #FFFFFF/#1E1E1E, chrome #F3F3F3,
 * accent #007ACC/#CCE8FF selection). */
const struct gui_theme ide_theme_white = {
    .desktop_bg = GUI_RGB(0xEA, 0xEA, 0xEA),

    .btn_bg = GUI_RGB(0xE1, 0xE1, 0xE1),
    .btn_bg_hot = GUI_RGB(0xCC, 0xE4, 0xF7),
    .btn_bg_active = GUI_RGB(0x99, 0xD1, 0xFF),
    .btn_fg = GUI_RGB(0x00, 0x00, 0x00),

    .hotkey_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .hotkey_key_fg = GUI_RGB(0x00, 0x7A, 0xCC),
    .hotkey_bg = GUI_RGB(0xCC, 0xE8, 0xFF),
    .hotkey_fg_hot = GUI_RGB(0x00, 0x00, 0x00),
    .hotkey_bg_hot = GUI_RGB(0x99, 0xD1, 0xFF),

    .menu_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .menu_bg = GUI_RGB(0xF3, 0xF3, 0xF3),
    /* #0078D7 - the Windows accent blue, with white ink on it (black would
     * score 4.1:1 against it, white 4.7:1, and white is what Windows itself
     * uses for a selected menu row). */
    .menu_fg_sel = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_bg_sel = GUI_RGB(0x00, 0x78, 0xD7),
    .menu_item_fg = GUI_RGB(0x00, 0x00, 0x00),
    .menu_item_bg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_fg_hot = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_bg_hot = GUI_RGB(0x00, 0x78, 0xD7),
    .menu_item_shortcut_fg = GUI_RGB(0x00, 0x66, 0x99),
    .menu_item_fg_disabled = GUI_RGB(0xA0, 0xA0, 0xA0),
    .menu_border_fg = GUI_RGB(0xCC, 0xCC, 0xCC),  /* light gray, not near-black */
    .menu_border_bg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_border_style = GUI_BORDER_DOUBLE,

    .box_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .box_bg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .box_border_style = GUI_BORDER_DOUBLE,

    /* #99B4D1 - the classic Windows window-frame blue-gray. Near-black
     * (#1E1E1E) frames around every window and dialog were the loudest thing
     * on screen in a light theme; this recedes into the chrome the way a real
     * light UI's frames do, while still separating a window from the desktop
     * behind it. */
    .window_border_fg = GUI_RGB(0x99, 0xB4, 0xD1),
    .window_border_bg = GUI_RGB(0xF0, 0xF0, 0xF0),
    .window_border_fg_dragging = GUI_RGB(0x00, 0x78, 0xD7),
    .window_border_fg_unfocused = GUI_RGB(0xC8, 0xC8, 0xC8),
    .window_border_style = GUI_BORDER_DOUBLE,
    .window_border_style_unfocused = GUI_BORDER_SINGLE,
    .window_border_style_docked = GUI_BORDER_DOUBLE,
    .window_border_style_docked_unfocused = GUI_BORDER_SINGLE,
    .window_close_bg = GUI_RGB(0xE8, 0x11, 0x23),
    .window_fg = GUI_RGB(0x00, 0x00, 0x00),
    .window_bg = GUI_RGB(0xF0, 0xF0, 0xF0),
    .modal_border_fg = GUI_RGB(0x99, 0xB4, 0xD1),
    .modal_border_bg = GUI_RGB(0xF0, 0xF0, 0xF0),
    .modal_border_style = GUI_BORDER_DOUBLE,
    .modal_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .modal_bg = GUI_RGB(0xF0, 0xF0, 0xF0),
    .label_fg = GUI_RGB(0x00, 0x00, 0x00),  /* plain black, not an accent - the
                                            * yellow the other two themes use
                                            * scores ~1.3:1 on this light
                                            * chrome, i.e. invisible. Kept a
                                            * shade off modal_fg (#1E1E1E)
                                            * deliberately: retheme_color()
                                            * tells a baked label apart from
                                            * baked body text by its value
                                            * alone, so the two slots must
                                            * never hold the same color. */
    .scrollbar_bg = GUI_RGB(0xF0, 0xF0, 0xF0),  /* matches the window border */
    .scrollbar_thumb_bg = GUI_RGB(0xC2, 0xC2, 0xC2),
    .scrollbar_thumb_hot_bg = GUI_RGB(0x8C, 0x8C, 0x8C),

    .input_bg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .input_bg_focus = GUI_RGB(0x00, 0x33, 0x66),  /* dark navy - unlike the
                                                  * other themes, White's own
                                                  * unfocused fill is already
                                                  * plain white, so a light
                                                  * focus tint wouldn't read
                                                  * as a change (and hides
                                                  * the caret); going dark
                                                  * instead keeps the caret/
                                                  * selection block (light
                                                  * blue) visible against it */
    .input_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .input_fg_focus = GUI_RGB(0xFF, 0xFF, 0xFF),  /* white - must stay
                                                  * readable against the dark
                                                  * input_bg_focus above,
                                                  * unlike the black used
                                                  * against the plain white
                                                  * unfocused fill */
    /* #ADD6FF - Visual Studio Light's actual text-selection color (also used
     * by <editor>, which reuses input_sel_*). Pale, so the selected text's
     * own color stays dark instead of the white used against the old darker
     * blue - otherwise it'd be unreadable against this light a background. */
    .input_sel_bg = GUI_RGB(0xAD, 0xD6, 0xFF),
    .input_sel_fg = GUI_RGB(0x00, 0x00, 0x00),

    .editor_bg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .editor_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .editor_keyword_fg = GUI_RGB(0x00, 0x00, 0xFF),   /* types/storage: blue */
    .editor_keyword2_fg = GUI_RGB(0xAF, 0x00, 0xDB),  /* control flow: purple */
    .editor_string_fg = GUI_RGB(0xA3, 0x15, 0x15),
    .editor_comment_fg = GUI_RGB(0x00, 0x80, 0x00),
    .editor_lint_fg = GUI_RGB(0x00, 0x00, 0xFF),  /* same blue as
                                                  * editor_keyword_fg - stands
                                                  * out from the green
                                                  * editor_comment_fg */
    .editor_linenum_fg = GUI_RGB(0x23, 0x78, 0x93),  /* teal-blue - VS Code
                                                     * Light+'s actual line-
                                                     * number color (matches the
                                                     * playground's gutter) */
    .editor_preproc_fg = GUI_RGB(0x80, 0x00, 0x80),
    .editor_attribute_fg = GUI_RGB(0x26, 0x7F, 0x99),
    .editor_sel_bg = GUI_RGB(0xAD, 0xD6, 0xFF),  /* #ADD6FF - VS Light's actual selection color */
    .editor_sel_fg = GUI_RGB(0x00, 0x00, 0x00),
    .editor_word_match_bg = GUI_RGB(0xE0, 0xE8, 0xF0),  /* pale blue-gray -
                                                        * reads against the
                                                        * white page without
                                                        * competing with the
                                                        * #ADD6FF selection */
    .editor_current_line_bg = GUI_RGB(0xF0, 0xF0, 0xF0),  /* subtle - close to
                                                          * VS Code Light's own
                                                          * current-line tint */
    .editor_breakpoint_fg = GUI_RGB(0xE5, 0x14, 0x00),  /* VS Code's own
                                                        * breakpoint red */
    .editor_exec_line_bg = GUI_RGB(0xDD, 0xF4, 0xDD),   /* pale green - reads
                                                        * against the white
                                                        * page like word_match_bg
                                                        * reads against blue */
    .editor_bracket_fg = {
        GUI_RGB(0x79, 0x5E, 0x26),  /* gold/brown */
        GUI_RGB(0x26, 0x7F, 0x99),  /* teal */
        GUI_RGB(0xB5, 0x60, 0x2B),  /* rust */
        GUI_RGB(0x5C, 0x6B, 0xC0),  /* indigo */
    },
    .editor_tag_fg = GUI_RGB(0x8B, 0x45, 0x13),  /* saddle brown - struct/
                                                 * union/enum tag names */
    .editor_number_fg = GUI_RGB(0x09, 0x86, 0x58),  /* teal-green - VS Code
                                                     * Light's actual numeric
                                                     * literal color */
    .editor_char_fg = GUI_RGB(0xAF, 0x00, 0x75),  /* deep pink - distinct from
                                                   * string_fg's dark red and
                                                   * keyword2_fg's purple */
    .editor_function_fg = GUI_RGB(0x79, 0x5E, 0x26),  /* dark gold - VS Code
                                                       * Light's actual function-
                                                       * name color */
    .editor_output_bg = GUI_RGB(0xF3, 0xF3, 0xF3),    /* light gray - VS Code
                                                       * Light's panel/chrome
                                                       * color, so the Output
                                                       * panel reads as recessed
                                                       * against the white
                                                       * document (editor_bg) */
    .editor_output_fg = GUI_RGB(0x1A, 0x73, 0xE8),    /* bright azure blue - the
                                                       * playground's Output text
                                                       * color; also the readable
                                                       * fallback for bright ANSI
                                                       * colors that would vanish
                                                       * on the gray
                                                       * editor_output_bg */

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same VS Code Light palette. */
    .md_heading_fg = GUI_RGB(0x00, 0x00, 0xFF),     /* same blue as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = GUI_RGB(0x00, 0x80, 0x00),  /* same green as
                                                     * editor_comment_fg */
    .md_code_fg = GUI_RGB(0xA3, 0x15, 0x15),        /* same dark red as
                                                     * editor_string_fg */
    .md_bold_fg = GUI_RGB(0x1E, 0x1E, 0x1E),        /* same as editor_fg */
    .md_link_fg = GUI_RGB(0x00, 0x66, 0xCC),        /* VS Code Light's actual
                                                     * hyperlink blue */
    .editor_diff_add_bg = GUI_RGB(0xE6, 0xFF, 0xEC),
    .editor_diff_remove_bg = GUI_RGB(0xFF, 0xEB, 0xE9),
    .md_code_bg = GUI_RGB(0xF6, 0xF8, 0xFA),        /* GitHub Light's actual
                                                     * code-block gray -
                                                     * distinct from both
                                                     * editor_bg and editor_
                                                     * current_line_bg */

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #CCE8FF accent as
     * every other "selected" state in this theme. */
    .listbox_fg = GUI_RGB(0x60, 0x60, 0x60),  /* dimmer than the editor's own
                                              * #1E1E1E body text - the
                                              * Folder panel is the biggest
                                              * listbox on screen and
                                              * shouldn't pull attention off
                                              * the code */
    .listbox_bg = GUI_RGB(0xF3, 0xF3, 0xF3),  /* VS Code Light's actual side
                                              * bar gray rather than the
                                              * editor's plain white, so the
                                              * panel recedes next to the
                                              * <editor> it sits beside */
    .listbox_sel_fg = GUI_RGB(0x00, 0x00, 0x00),
    .listbox_sel_bg = GUI_RGB(0xCC, 0xE8, 0xFF),
    .listbox_sel_inactive_fg = GUI_RGB(0x1E, 0x1E, 0x1E),
    .listbox_sel_inactive_bg = GUI_RGB(0xE0, 0xE0, 0xE0),

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = GUI_RGB(0x00, 0x00, 0xFF),   /* matches this theme's
                                                       * own editor_keyword_fg */
    .project_icon_h_fg = GUI_RGB(0x00, 0x80, 0x00),   /* dark green, legible
                                                       * against a light bg */
    .project_icon_md_fg = GUI_RGB(0x00, 0x66, 0xCC),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - VS Code Light's actual error/warning/
     * info squiggle colors, so they read as authentically part of this
     * theme. */
    .diag_error_fg = GUI_RGB(0xE5, 0x14, 0x00),
    .diag_warning_fg = GUI_RGB(0xBF, 0x88, 0x03),
    .diag_info_fg = GUI_RGB(0x1A, 0x85, 0xFF),
    /* Pale tints of each fg - left unset these are 0, a black bar on a white editor. */
    .diag_error_bg = GUI_RGB(0xFD, 0xE7, 0xE9),
    .diag_warning_bg = GUI_RGB(0xFF, 0xF4, 0xCE),
    .diag_info_bg = GUI_RGB(0xE5, 0xF1, 0xFB),
};

/* "Nebula" - a deep indigo night palette (editor #1A1B26, text #C0CAF5,
 * accent #7AA2F7): violet keywords, blue function names, green strings and
 * orange literals, with the whole chrome tinted the same indigo instead of
 * a neutral gray. */
const struct gui_theme ide_theme_nebula = {
    /* NOTE: anything drawn ON the #7AA2F7 accent uses the dark #1A1B26 ink,
     * not white - white scores 2.4:1 against this blue, the dark ink 8.7:1. */
    .desktop_bg = GUI_RGB(0x16, 0x16, 0x1E),

    .btn_bg = GUI_RGB(0x2A, 0x2E, 0x40),
    .btn_bg_hot = GUI_RGB(0x3B, 0x42, 0x61),
    .btn_bg_active = GUI_RGB(0x7A, 0xA2, 0xF7),
    .btn_fg = GUI_RGB(0xC0, 0xCA, 0xF5),

    .hotkey_fg = GUI_RGB(0x1A, 0x1B, 0x26),
    .hotkey_key_fg = GUI_RGB(0x8C, 0x2E, 0x2E),
    .hotkey_bg = GUI_RGB(0x7A, 0xA2, 0xF7),
    .hotkey_fg_hot = GUI_RGB(0x1A, 0x1B, 0x26),
    .hotkey_bg_hot = GUI_RGB(0x9E, 0xBC, 0xFF),

    .menu_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .menu_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .menu_fg_sel = GUI_RGB(0x1A, 0x1B, 0x26),
    .menu_bg_sel = GUI_RGB(0x7A, 0xA2, 0xF7),
    .menu_item_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .menu_item_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .menu_item_fg_hot = GUI_RGB(0x1A, 0x1B, 0x26),
    .menu_item_bg_hot = GUI_RGB(0x7A, 0xA2, 0xF7),
    .menu_item_shortcut_fg = GUI_RGB(0x7D, 0x86, 0xA8),
    .menu_item_fg_disabled = GUI_RGB(0x56, 0x5F, 0x89),
    .menu_border_fg = GUI_RGB(0x3B, 0x42, 0x61),
    .menu_border_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .menu_border_style = GUI_BORDER_SINGLE,

    .box_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .box_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .box_border_style = GUI_BORDER_DOUBLE,

    .window_border_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .window_border_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .window_border_fg_dragging = GUI_RGB(0x7A, 0xA2, 0xF7),
    .window_border_fg_unfocused = GUI_RGB(0x56, 0x5F, 0x89),
    .window_border_style = GUI_BORDER_DOUBLE,
    .window_border_style_unfocused = GUI_BORDER_SINGLE,
    .window_border_style_docked = GUI_BORDER_SINGLE,
    .window_border_style_docked_unfocused = GUI_BORDER_SINGLE,
    .window_close_bg = GUI_RGB(0xF7, 0x76, 0x8E),
    .window_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .window_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .modal_border_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .modal_border_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .modal_border_style = GUI_BORDER_DOUBLE,
    .modal_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .modal_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .label_fg = GUI_RGB(0x7A, 0xA2, 0xF7),  /* the theme's blue accent */
    .scrollbar_bg = GUI_RGB(0x1F, 0x22, 0x33),
    .scrollbar_thumb_bg = GUI_RGB(0x3B, 0x42, 0x61),
    .scrollbar_thumb_hot_bg = GUI_RGB(0x56, 0x5F, 0x89),

    .input_bg = GUI_RGB(0x2A, 0x2E, 0x40),
    .input_bg_focus = GUI_RGB(0x16, 0x16, 0x1E),  /* darker than input_bg so
                                                  * focus reads without a blue
                                                  * tint, which would collide
                                                  * with input_sel_bg below */
    .input_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .input_fg_focus = GUI_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = GUI_RGB(0x2E, 0x3C, 0x64),
    .input_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = GUI_RGB(0x1A, 0x1B, 0x26),
    .editor_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .editor_keyword_fg = GUI_RGB(0xF7, 0x76, 0x8E),   /* types/storage: rose -
                                                      * the same color the
                                                      * screenshot gives
                                                      * `return`/`const` */
    .editor_keyword2_fg = GUI_RGB(0xBB, 0x9A, 0xF7),  /* control flow: violet */
    .editor_string_fg = GUI_RGB(0x9E, 0xCE, 0x6A),
    .editor_comment_fg = GUI_RGB(0x56, 0x5F, 0x89),
    .editor_lint_fg = GUI_RGB(0xE0, 0xAF, 0x68),  /* amber - stands out from the
                                                  * dim indigo comment gray */
    .editor_linenum_fg = GUI_RGB(0x3B, 0x42, 0x61),
    .editor_preproc_fg = GUI_RGB(0xBB, 0x9A, 0xF7),
    .editor_attribute_fg = GUI_RGB(0x7D, 0xCF, 0xFF),
    .editor_sel_bg = GUI_RGB(0x2E, 0x3C, 0x64),
    .editor_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = GUI_RGB(0x28, 0x2D, 0x43),
    .editor_current_line_bg = GUI_RGB(0x21, 0x23, 0x33),  /* one subtle step up
                                                          * from editor_bg */
    .editor_breakpoint_fg = GUI_RGB(0xE0, 0x5A, 0x5A),  /* red, distinct from
                                                        * the rose editor_keyword_fg */
    .editor_exec_line_bg = GUI_RGB(0x1F, 0x33, 0x2A),   /* green step up from
                                                        * editor_bg, same idea
                                                        * as editor_current_line_bg */
    .editor_bracket_fg = {
        GUI_RGB(0xE0, 0xAF, 0x68),  /* amber */
        GUI_RGB(0xF7, 0x76, 0x8E),  /* rose */
        GUI_RGB(0x73, 0xDA, 0xCA),  /* teal */
        GUI_RGB(0x7A, 0xA2, 0xF7),  /* blue */
    },
    .editor_tag_fg = GUI_RGB(0x73, 0xDA, 0xCA),  /* teal - deliberately NOT the
                                                 * cyan editor_keyword_fg, so a
                                                 * tag name reads apart from
                                                 * the `struct` before it */
    .editor_number_fg = GUI_RGB(0xFF, 0x9E, 0x64),  /* orange - numeric and
                                                     * NULL-style literals */
    .editor_char_fg = GUI_RGB(0xFF, 0xC7, 0x77),    /* warm gold - distinct from
                                                     * the green string_fg and
                                                     * from the rose now used
                                                     * by editor_keyword_fg */
    .editor_function_fg = GUI_RGB(0x7A, 0xA2, 0xF7),  /* blue - the same accent
                                                       * the chrome uses */
    .editor_output_bg = GUI_RGB(0x16, 0x16, 0x1E),  /* one step darker than
                                                    * editor_bg, so Output
                                                    * reads as recessed */
    .editor_output_fg = GUI_RGB(0xC0, 0xCA, 0xF5),

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same indigo palette. */
    .md_heading_fg = GUI_RGB(0x7A, 0xA2, 0xF7),
    .md_blockquote_fg = GUI_RGB(0x56, 0x5F, 0x89),
    .md_code_fg = GUI_RGB(0x9E, 0xCE, 0x6A),
    .md_bold_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .md_link_fg = GUI_RGB(0x73, 0xDA, 0xCA),
    .editor_diff_add_bg = GUI_RGB(0x21, 0x3B, 0x2E),
    .editor_diff_remove_bg = GUI_RGB(0x3B, 0x21, 0x30),
    .md_code_bg = GUI_RGB(0x1F, 0x22, 0x33),

    /* <listbox> - same body colors as <editor>/<input>, selection reuses the
     * same #7AA2F7 accent as every other "selected" state in this theme. */
    .listbox_fg = GUI_RGB(0x9A, 0xA5, 0xCE),  /* dimmer than editor_fg: the
                                              * Folder panel is the biggest
                                              * list on screen and shouldn't
                                              * pull attention off the code */
    .listbox_bg = GUI_RGB(0x1A, 0x1B, 0x26),
    .listbox_sel_fg = GUI_RGB(0x1A, 0x1B, 0x26),
    .listbox_sel_bg = GUI_RGB(0x7A, 0xA2, 0xF7),
    .listbox_sel_inactive_fg = GUI_RGB(0xC0, 0xCA, 0xF5),
    .listbox_sel_inactive_bg = GUI_RGB(0x2A, 0x2E, 0x40),

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = GUI_RGB(0x7A, 0xA2, 0xF7),   /* this theme's own accent */
    .project_icon_h_fg = GUI_RGB(0x9E, 0xCE, 0x6A),   /* matches md_code_fg's green */
    .project_icon_md_fg = GUI_RGB(0x73, 0xDA, 0xCA),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - the palette's own red/amber/blue, so the
     * squiggles read as part of this theme. */
    .diag_error_fg = GUI_RGB(0xDB, 0x4B, 0x4B),
    .diag_warning_fg = GUI_RGB(0xE0, 0xAF, 0x68),
    .diag_info_fg = GUI_RGB(0x0D, 0xB9, 0xD7),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = GUI_RGB(0x3D, 0x24, 0x2D),
    .diag_warning_bg = GUI_RGB(0x3E, 0x36, 0x32),
    .diag_info_bg = GUI_RGB(0x18, 0x37, 0x46),
};

/* "Xcode Dark" - Xcode's own Default (Dark) editor palette (editor #292A30,
 * text #FFFFFF, current line #2F3239, selection #646F83): pink keywords,
 * salmon strings, khaki numbers/chars, orange preprocessor, gray-blue
 * comments, mint type names and teal function names. Chrome is Xcode's
 * dark navigator/inspector graphite with macOS's system blue as accent. */
const struct gui_theme ide_theme_xcode_dark = {
    .desktop_bg = GUI_RGB(0x1F, 0x1F, 0x24),

    .btn_bg = GUI_RGB(0x3D, 0x3E, 0x45),
    .btn_bg_hot = GUI_RGB(0x0A, 0x84, 0xFF),
    .btn_bg_active = GUI_RGB(0x00, 0x58, 0xD0),
    .btn_fg = GUI_RGB(0xE5, 0xE5, 0xEA),

    .hotkey_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .hotkey_key_fg = GUI_RGB(0xD0, 0xBF, 0x69),  /* the editor's khaki literal color */
    .hotkey_bg = GUI_RGB(0x00, 0x58, 0xD0),
    .hotkey_fg_hot = GUI_RGB(0xFF, 0xFF, 0xFF),
    .hotkey_bg_hot = GUI_RGB(0x0A, 0x84, 0xFF),

    .menu_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .menu_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .menu_fg_sel = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_bg_sel = GUI_RGB(0x00, 0x58, 0xD0),
    .menu_item_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .menu_item_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .menu_item_fg_hot = GUI_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_bg_hot = GUI_RGB(0x00, 0x58, 0xD0),
    .menu_item_shortcut_fg = GUI_RGB(0x8E, 0x8E, 0x93),
    .menu_item_fg_disabled = GUI_RGB(0x63, 0x63, 0x66),
    .menu_border_fg = GUI_RGB(0x4A, 0x4A, 0x4F),
    .menu_border_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .menu_border_style = GUI_BORDER_SINGLE,

    .box_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .box_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .box_border_style = GUI_BORDER_DOUBLE,

    .window_border_fg = GUI_RGB(0xC7, 0xC7, 0xCC),
    .window_border_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .window_border_fg_dragging = GUI_RGB(0x0A, 0x84, 0xFF),
    .window_border_fg_unfocused = GUI_RGB(0x63, 0x63, 0x66),
    .window_border_style = GUI_BORDER_DOUBLE,
    .window_border_style_unfocused = GUI_BORDER_SINGLE,
    .window_border_style_docked = GUI_BORDER_SINGLE,
    .window_border_style_docked_unfocused = GUI_BORDER_SINGLE,
    .window_close_bg = GUI_RGB(0xFF, 0x5F, 0x57),  /* macOS's own red traffic light */
    .window_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .window_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .modal_border_fg = GUI_RGB(0xC7, 0xC7, 0xCC),
    .modal_border_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .modal_border_style = GUI_BORDER_DOUBLE,
    .modal_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .modal_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .label_fg = GUI_RGB(0x5A, 0xC8, 0xFA),  /* macOS system teal-blue */
    .scrollbar_bg = GUI_RGB(0x2D, 0x2E, 0x33),
    .scrollbar_thumb_bg = GUI_RGB(0x5A, 0x5A, 0x5F),
    .scrollbar_thumb_hot_bg = GUI_RGB(0x82, 0x82, 0x88),

    .input_bg = GUI_RGB(0x3D, 0x3E, 0x45),
    .input_bg_focus = GUI_RGB(0x1F, 0x1F, 0x24),  /* darker than input_bg, not
                                                  * a blue tint, so the blue
                                                  * input_sel_bg stays visible */
    .input_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .input_fg_focus = GUI_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = GUI_RGB(0x64, 0x6F, 0x83),
    .input_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = GUI_RGB(0x29, 0x2A, 0x30),
    .editor_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .editor_keyword_fg = GUI_RGB(0xFC, 0x5F, 0xA3),   /* Xcode uses one pink for
                                                      * every keyword... */
    .editor_keyword2_fg = GUI_RGB(0xFC, 0x5F, 0xA3),  /* ...control flow too */
    .editor_string_fg = GUI_RGB(0xFC, 0x6A, 0x5D),
    .editor_comment_fg = GUI_RGB(0x6C, 0x79, 0x86),
    .editor_lint_fg = GUI_RGB(0x5A, 0xC8, 0xFA),  /* stands out from the
                                                  * gray-blue comment color */
    .editor_linenum_fg = GUI_RGB(0x5C, 0x5F, 0x66),
    .editor_preproc_fg = GUI_RGB(0xFD, 0x8F, 0x3F),
    .editor_attribute_fg = GUI_RGB(0x67, 0xB7, 0xA4),
    .editor_sel_bg = GUI_RGB(0x64, 0x6F, 0x83),  /* Xcode Dark's actual selection */
    .editor_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = GUI_RGB(0x3C, 0x40, 0x48),
    .editor_current_line_bg = GUI_RGB(0x2F, 0x32, 0x39),  /* Xcode Dark's actual
                                                          * current-line tint */
    .editor_breakpoint_fg = GUI_RGB(0x3F, 0x80, 0xF5),  /* Xcode's breakpoints
                                                        * are blue chips */
    .editor_exec_line_bg = GUI_RGB(0x2E, 0x4A, 0x3C),   /* green step up from
                                                        * editor_bg */
    .editor_bracket_fg = {
        GUI_RGB(0xFD, 0x8F, 0x3F),  /* orange */
        GUI_RGB(0xD0, 0xBF, 0x69),  /* khaki */
        GUI_RGB(0x9E, 0xF1, 0xDD),  /* mint */
        GUI_RGB(0xDA, 0xBA, 0xFF),  /* lavender */
    },
    .editor_tag_fg = GUI_RGB(0x9E, 0xF1, 0xDD),  /* mint - Xcode's project
                                                 * type-name color */
    .editor_number_fg = GUI_RGB(0xD0, 0xBF, 0x69),
    .editor_char_fg = GUI_RGB(0xD0, 0xBF, 0x69),  /* Xcode colors chars like
                                                  * numbers, not like strings */
    .editor_function_fg = GUI_RGB(0x67, 0xB7, 0xA4),  /* teal - Xcode's project
                                                       * function-name color */
    .editor_output_bg = GUI_RGB(0x1F, 0x1F, 0x24),  /* one step darker than
                                                    * editor_bg, like Xcode's
                                                    * console */
    .editor_output_fg = GUI_RGB(0xE5, 0xE5, 0xEA),

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * so Markdown reads as part of the same palette. */
    .md_heading_fg = GUI_RGB(0xFC, 0x5F, 0xA3),
    .md_blockquote_fg = GUI_RGB(0x6C, 0x79, 0x86),
    .md_code_fg = GUI_RGB(0xD0, 0xBF, 0x69),
    .md_bold_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .md_link_fg = GUI_RGB(0x5A, 0xC8, 0xFA),
    .editor_diff_add_bg = GUI_RGB(0x1C, 0x33, 0x20),
    .editor_diff_remove_bg = GUI_RGB(0x3A, 0x1E, 0x1E),
    .md_code_bg = GUI_RGB(0x2D, 0x2E, 0x33),

    /* <listbox> - Xcode's navigator: a shade darker than the editor, with
     * the same system blue as every other "selected" state in this theme. */
    .listbox_fg = GUI_RGB(0xD1, 0xD1, 0xD6),  /* dimmer than editor_fg so the
                                              * Folder panel doesn't pull
                                              * attention off the code */
    .listbox_bg = GUI_RGB(0x26, 0x26, 0x2B),
    .listbox_sel_fg = GUI_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_bg = GUI_RGB(0x00, 0x58, 0xD0),
    .listbox_sel_inactive_fg = GUI_RGB(0xE5, 0xE5, 0xEA),
    .listbox_sel_inactive_bg = GUI_RGB(0x3D, 0x3E, 0x45),

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = GUI_RGB(0x9E, 0xF1, 0xDD),   /* matches editor_tag_fg */
    .project_icon_h_fg = GUI_RGB(0xDA, 0xBA, 0xFF),   /* lavender bracket color */
    .project_icon_md_fg = GUI_RGB(0xFD, 0x8F, 0x3F),  /* matches editor_preproc_fg */

    /* <editor> inline diagnostics - Xcode's own issue-navigator red/yellow
     * and the system teal-blue for notes. */
    .diag_error_fg = GUI_RGB(0xFF, 0x4B, 0x4B),
    .diag_warning_fg = GUI_RGB(0xFF, 0xC6, 0x27),
    .diag_info_fg = GUI_RGB(0x5A, 0xC8, 0xFA),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = GUI_RGB(0x50, 0x30, 0x35),
    .diag_warning_bg = GUI_RGB(0x50, 0x46, 0x2E),
    .diag_info_bg = GUI_RGB(0x32, 0x46, 0x54),
};
