my $query=&trim($ARGV[0]);
$query =~ s/\t/ /g;	# replace tabs with spaces
$query =~ s/\'\f\a\e//g;	# strip out single quotes (apostrophes), form feeds, bells, escapes
$query =~ s/\"/ \" /g;
$query =~ s/\+/ \+ /g;

my @requires=();
my @combines=();

while (length($query)>0) {
	if (substr($query, 0, 1) eq "+") {
		$query=&trim(substr($query, 1));	# skip 1 char
		my $nxtTerm = &next_term;
		if ($nxtTerm ne "") {
			push(@requires, $nxtTerm);
		}
	} else {
        my $nxtTerm = &next_term;
        if ($nxtTerm ne "") {
			push(@combines, $nxtTerm);
		}
	}
}

my $return = "@requires @combines";
if (($#requires + $#combines)>-1) {
	$return = "#combine($return)";
}

if ($#requires > 0) {
	$return = "#filreq(#band(@requires) $return )";
} else {
	if ($#requires > -1) {
		$return = "#filreq(@requires $return )";
	}
}
print $return;

sub next_term {
	my $term="";
	if (substr($query, 0, 1) eq '"') {
		my $indx = index($query, '"', 1);
		if ($indx==-1) {
			$query=&trim(substr($query, 1));	# skip this leading quote since no closing
			return &next_term;
		} else {
			$term = substr($query, 1, $indx-1);
			$query=substr($query, $indx+1);
		}
	} else {
		my $indx = index($query, ' ');	# look for next whitespace
		if ($indx>-1) {
			$term = substr($query, 0, $indx);
			$query=substr($query, $indx);
		} else {
			$term = $query;
			$query = "";
		}
	}
	$query = &trim($query);
	$term = &trim($term);
	$term =~ s/\+//g;	# remove any extra pluses inside
	my @words = split(/ /, $term);
	my @expanded_words=();	# build a new array of expanded words
	foreach $word (@words) {
		if ($word =~ m/[^\w]/) {
			my $space = $word;
			my $blank = $word;
			$space =~ s/[^\w]/ /g;
			$blank =~ s/[^\w]//g;
			$space = &trim($space);
			$blank = &trim($blank);
			if ($space ne $blank) {
				push(@expanded_words, "$space $blank");
			} else {
				if ($space ne "") {
					push(@expanded_words, $space);
				}
			}
		} else {
			push(@expanded_words, $word);
		}
	}
	$term = "@expanded_words";	# combine back with a space
	if ($#expanded_words > 0) {
		$term = "#1($term)";
	}
	return $term;
}

sub trim {
  my $string = shift;
  for ($string) {
    s/^\s+//;
    s/\s+$//;
  }
  return $string;
}
