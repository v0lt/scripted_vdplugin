/*
 * Copyright (C) 2025 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "CVapourSynth.h"

// https://docs.python.org/3.14/reference/lexical_analysis.html#keywords
const char coKeywordsDefault[] =
	"None "
	"True "
	"and "
	"as "
	"assert "
	"async "
	"await "
	"break "
	"class "
	"continue "
	"def "
	"del "
	"elif "
	"else "
	"except "
	"finally "
	"for "
	"from "
	"global "
	"if "
	"import "
	"in "
	"is "
	"lambda "
	"nonlocal "
	"not "
	"or "
	"pass "
	"raise "
	"return "
	"try "
	"while "
	"with "
	"yield ";

CVapourSynth::CVapourSynth()
{
	coKeywords = _strdup(coKeywordsDefault);
}
