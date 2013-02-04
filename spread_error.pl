#!/usr/bin/perl

# format spread error codes and descriptions, extracted from man pages, into xml

use strict;

my ($func, $code, $desc);
print "<spread-errors>\n";
while (<>) {
    chomp;
    next if /^BUGS$/;
    if (/^\s*$/ || eof()) {
	next unless $func && $code && $desc;
	$desc =~ s/"/&quot;/g;
	$desc =~ s/\s+/ /g;
	$desc =~ s/^\s+|\s+$//g;
	print qq{<error func="$func" code="$code" desc="$desc" />\n};
	$code = $desc = undef;
    }
    elsif (/^Function:\s(.+)/) {
	$func = $1;
    }
    elsif (/^RETURN\sVALUES/) {
	# skip this and following non-blank lines.
	# they describe return successful return values, not errors.
	do { $_ = <> } until (/^\s*$/);
	redo;
    }
    elsif (/^\s+([A-Z_]+)$/) {
	$code = $1;
    }
    else {
	$desc .= ' ' . $_;
    }
}
print "</spread-errors>\n";
