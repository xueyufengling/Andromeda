# This is a internal ansi style text cmake file, mainly used in message();
# Use include(${LIBCXXBASE_CMAKE_DIR}/echo.cmake) to include this file.

include("${LIBCXXBASE_CMAKE_DIR}/exec.cmake")

# Enable ANSI text style for the current terminal.
set(CLICOLOR_FORCE 1 CACHE STRING "Enable output ANSI style")

# Print colored text, text_style is separated with ' '. This function won't print in new line.
# Available text styles are:
# style: bold
# color: normal, black, red, green, yellow, blue, magenta, cyan, white
function(echo_color text_style)
	string(REPLACE "_" ";--" style_args ${text_style})
	set(style_args --${style_args})
	string(JOIN "" text ${ARGN})
	# CLICOLOR_FORCE=1 enables color output, used by cmake -E cmake_echo_color.
	cmake(-E env CLICOLOR_FORCE=1 cmake -E cmake_echo_color ${style_args} --no-newline "${text}")
endfunction()

# Print colored text in new line. '\n' is always the end of output, just like message().
function(echo_color_ln text_style)
	echo_color(${text_style} ${ARGN} "\n")
endfunction()

# Print colored message starts with "-- ", alternative for message()
function(echo_color_msg text_style)
	echo_color_ln(${text_style} "-- " ${ARGN})
endfunction()

# ANSI style for text for message(). pass extra parames as a list of codes.
function(ansi_style style)
	string(ASCII 27 e)
	string(JOIN ";" codes ${ARGN})
	set(${style} "${e}[${codes}m" PARENT_SCOPE)
endfunction()

# Foreground color 256.
function(ansi_fg_color_256 style color_code)
	ansi_style(color_256 38 5 ${color_code})
	set(${style} ${color_256} PARENT_SCOPE)
endfunction()

# Foreground color RGB.
function(ansi_fg_color_rgb style)
	ansi_style(color_rgb 38 2 ${ARGN})
	set(${style} ${color_rgb} PARENT_SCOPE)
endfunction()

# Background color 256.
function(ansi_bg_color_256 style color_code)
	ansi_style(color_256 48 5 ${color_code})
	set(${style} ${color_256} PARENT_SCOPE)
endfunction()

# Background color RGB.
function(ansi_bg_color_rgb style)
	ansi_style(color_rgb 48 2 ${ARGN})
	set(${style} ${color_rgb} PARENT_SCOPE)
endfunction()

ansi_style(ansi_reset 0)

ansi_style(ansi_bold 1)
ansi_style(ansi_faint 2)
ansi_style(ansi_italic 3)
ansi_style(ansi_underline 4)
ansi_style(ansi_slow_blink 5)
ansi_style(ansi_inverse 7)
ansi_style(ansi_strike 9)
ansi_style(ansi_not_bold 21)
ansi_style(ansi_doublely_underlined 21)
ansi_style(ansi_normal_intensity 22)
ansi_style(ansi_not_italic 23)
ansi_style(ansi_not_underline 24)
ansi_style(ansi_not_blink 25)
ansi_style(ansi_not_inverse 27)
ansi_style(ansi_reveal 28)
ansi_style(ansi_not_strike 29)

ansi_style(ansi_fg_black 30)
ansi_style(ansi_fg_red 31)
ansi_style(ansi_fg_green 32)
ansi_style(ansi_fg_yellow 33)
ansi_style(ansi_fg_blue 34)
ansi_style(ansi_fg_magenta 35)
ansi_style(ansi_fg_cyan 36)
ansi_style(ansi_fg_white 37)
ansi_style(ansi_fg_reset 39)

ansi_style(ansi_fg_bright_black 90)
ansi_style(ansi_fg_bright_red 91)
ansi_style(ansi_fg_bright_green 92)
ansi_style(ansi_fg_bright_yellow 93)
ansi_style(ansi_fg_bright_blue 94)
ansi_style(ansi_fg_bright_magenta 95)
ansi_style(ansi_fg_bright_cyan 96)
ansi_style(ansi_fg_bright_white 97)

ansi_style(ansi_bg_black 41)
ansi_style(ansi_bg_red 41)
ansi_style(ansi_bg_green 42)
ansi_style(ansi_bg_yellow 43)
ansi_style(ansi_bg_blue 44)
ansi_style(ansi_bg_magenta 45)
ansi_style(ansi_bg_cyan 46)
ansi_style(ansi_bg_white 47)

ansi_style(ansi_bg_bright_black 100)
ansi_style(ansi_bg_bright_red 101)
ansi_style(ansi_bg_bright_green 102)
ansi_style(ansi_bg_bright_yellow 103)
ansi_style(ansi_bg_bright_blue 104)
ansi_style(ansi_bg_bright_magenta 105)
ansi_style(ansi_bg_bright_cyan 106)
ansi_style(ansi_bg_bright_white 107)

ansi_style(ansi_overlined 53)
ansi_style(ansi_not_overlined 55)

set(ansi_fatal "${ansi_fg_bright_white}${ansi_bg_bright_red}")
set(ansi_error "${ansi_fg_red}")
set(ansi_warn "${ansi_fg_yellow}")
set(ansi_detailed "${ansi_fg_bright_green}")
set(ansi_important "${ansi_fg_magenta}")
set(ansi_prompt "${ansi_fg_blue}")

# Print to stdout with ANSI style enabled, no new line.
function(echo)
	string(JOIN "" text ${ARGN})
	printf("${ansi_reset}${text}${ansi_reset}")
endfunction()

function(echo_ln)
	string(JOIN "" text ${ARGN})
	echo("${text}\n")
endfunction()

function(echo_msg ansi_text_style)
	string(JOIN "" text ${ARGN})
	echo_ln("${ansi_text_style}-- ${text}")
endfunction()

function(echo_fatal)
	echo_msg("${ansi_fatal}" ${ARGN})
endfunction()

function(echo_error)
	echo_msg("${ansi_error}" ${ARGN})
endfunction()

function(echo_warn)
	echo_msg("${ansi_warn}" ${ARGN})
endfunction()

function(echo_detailed)
	echo_msg("${ansi_detailed}" ${ARGN})
endfunction()

function(echo_important)
	echo_msg("${ansi_important}" ${ARGN})
endfunction()

function(echo_prompt)
	echo_msg("${ansi_prompt}" ${ARGN})
endfunction()