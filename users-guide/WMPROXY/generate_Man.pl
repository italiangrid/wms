#!/usr/bin/perl

%tags = ('VERB' => '1', 
         'ENDVERB' => '1',
	 'INLINEVERB' => '1',
	 'ENDINLINEVERB' => '1',
	 'ENUM' => '1',
	 'ENDENUM' => '1',
	 'ITEMIZE' => '1',
	 'ENDITEMIZE' => '1',
	 'NEWLINE' => '1',
	 'ITEM' => '1',
	 'OPT' => '1',
	 'ENDOPT' => '1',
	 'USC' => '1',
	 'DOLLAR' => '1',
	 'REF', => '1',
	 'ENDREF' => '1',
	 'PAGEREF' => '1',
	 'ENDPAGEREF' => '1',
	 'BF' => '1',
	 'ENDBF' => '1' );

sub check_tag {
  $line = $_[0];
  $_row  = $_[1];
  $filename = $_[2];
  
  @pieces = split ' ', $line;
  foreach(@pieces) {
    $piece = $_;
    if($piece =~ m/<<<([A-Z]+)>>>/) {
      if(exists $tags{$1}) {
        ;
      } else {
        print "Error at row $_row file [$filename]: TAG <<<$1>>> is not allowed";
	die;
      }
    }
  }
  return;
}

sub mangletags {
  $line = $_[0];
  if($line =~ m/^<<<ITEM>>>\s+/) {
    #print "TROVATO\n";
    $line =~ s/<<<ITEM>>>\s+/\n\n.SP\n.B -\n/g;
    #print "line=[$line]\n";
    #exit;
  }
 
  $line =~ s/<<<SPACE>>>/ /g;
  $line =~ s/<<<VERB>>>//g;
  $line =~ s/<<<ENDVERB>>>//g;
  $line =~ s/<<<NEWLINE>>>/\n\n\n\.SP\n/g;
  $line =~ s/<<<ITEM>>>/\n\n.SP\n.B -\n/g;
  $line =~ s/<<<ITEM>>>\s+/\n\n.SP\n.B -\n/g;
  $line =~ s/<<<ENUM>>>/\n/g;
  $line =~ s/<<<ENDENUM>>>/\n.SP\n.SP\n/g;
  $line =~ s/<<<ITEMIZE>>>//g;
  $line =~ s/<<<ENDITEMIZE>>>//g;
  $line =~ s/<<<INLINEVERB>>>//g;
  $line =~ s/<<<ENDINLINEVERB>>>//g;
  $line =~ s/<<<USC>>>/_/g;
  $line =~ s/<<<DOLLAR>>>/\$/g;
  $line =~ s/<<<REF>>>/\(/g;
  $line =~ s/<<<ENDREF>>>/\)/g;
  $line =~ s/<<<PAGEREF>>>/\(/g;
  $line =~ s/<<<ENDPAGEREF>>>/\)/g;
  $line =~ s/<<<BF>>>/\\.B /g;
  $line =~ s/<<<ENDBF>>>/\n/g;
  $line =~ s/'/"/g;
  return $line;
}

$CMDNAME = shift;
$row =0;
@final_text = ();

#push @final_text, "\n\.B\n$CMDNAME\n\\label{$CMDNAME}\n\n\\medskip\n\\textbf{$CMDNAME}\n\\smallskip\n\n";

push @final_text, ".TH " . uc($CMDNAME) . " \"1\" \"" . uc($CMDNAME) . "\" \"GLITE User Guide\"\n";
push @final_text, ".SH NAME:\n$CMDNAME\n\n.SH SYNOPSIS\n";
##
#
#
# GENERATE SYNOPSIS 
#
#
##

$sinopsis = `cat $CMDNAME/synopsis.txt`;
#$sinopsis =~ s/_/\\_/g;
chomp($sinopsis);
check_tag($sinopsis, "1", "$CMDNAME/synopsis.txt");
$sinopsis = mangletags($sinopsis);

push @final_text, "\n.B $CMDNAME $sinopsis \n";

# foreach(@final_text) {
#   print;
# }

# exit;

@option_verb = `cat $CMDNAME/options_verb.txt`;

push @final_text, "\n";

foreach(@option_verb) {
  chomp;
  $line = $_;
  check_tag( $line, "1", "$CMDNAME/options_verb.txt" );

  $line = mangletags( $line );
  push @final_text, $line."\n";
}

# foreach(@final_text) {
#   print;
# }

# exit;

##
#
#
# GENERATE DESCRIPTION 
#
#
##

push @final_text, "\n.SP\n.SH DESCRIPTION\n.SP\n.SP\n\n";

@desc_lines = `cat $CMDNAME/desc.txt`;
$verbatim = 0;
foreach(@desc_lines) {
  $row++;
  $line = $_;
  chomp($line);
  
  check_tag( $line, $row, "$CMDNAME/desc.txt" );
  
#  $line =~ s/^\s+//;
#  $line =~ s/\s+$//;
#  if($line =~ m/<<<VERB>>>/) {
#    $verbatim = 1;
#  }
#  if($line =~ m/<<<ENDVERB>>>/) {
#    $verbatim = 0;
#  }
#  if(($line =~ m/<<<VERB>>>/) && ($line =~ m/<<<ENDVERB>>>/) ) {
#    die "Error at row $row: <<<VERB>>> and <<<ENDVERB>>> must be on different lines";
#  }
#  if($verbatim==0) {
#    $line =~ s/--/-{}-/g;
#    #$line =~ s/_/\\_/g;
#    #$line =~ s/\$/\\\$/g;
#  }
  
  $line = mangletags($line);
  push @final_text, $line;
}


# foreach(@final_text) {
#   print;
# }

# exit;


##
#
#
# GENERATE OPTIONS
#
#
##
@option_description = `cat common/common_opts_1.txt $CMDNAME/opts.txt`;

push @final_text, "\n\.SH OPTIONS \n";

foreach(@option_description) {
    chomp;
  $row++;
  $line = $_;
  
  check_tag( $line, $row, "common/common_opts_1.txt or $CMDNAME/opts.txt" );

  $line = mangletags($line);

  if($line =~ m/^<<<OPT>>>/) {
    $line =~ s/^<<<OPT>>>//;
    chomp($line);

    push @final_text, ".B $line\n\n.IP\n";
    next;
  }
  if($line =~ m/^<<<ENDOPT>>>/) {
    $line =~ s/^<<<ENDOPT>>>//;
    chomp($line);

    push @final_text, "\n.PP\n";
    next;
  }
  
  push @final_text, $line;
}

# foreach(@final_text) {
#   print;
# }

# exit;

##
#
#
# GENERATE EXAMPLES
#
#
##
@examples = `cat $CMDNAME/examples.txt`;
if($#examples > 0 ) {
  push @final_text, "\n\.SH EXAMPLES\n.SP\n\n";

  foreach(@examples) 
  {
    chomp;
    $row++;
    $line = $_;
    check_tag( $line, $row, "$CMDNAME/examples.txt" );
    
    $line = mangletags($line);
    push @final_text, $line;
  }
}
##
#
#
#GENERATE FILES
#
#
##
@files = `cat $CMDNAME/files.txt`;

push @final_text, "\n\.SH FILES \n.SP\n\n";


foreach(@files) {
    chomp;
  $row++;
  $line = $_;
  
  check_tag( $line, $row, "$CMDNAME/files.txt" );

  $line = mangletags($line);
  push @final_text, $line;
}

##
#
#
# GENERATE ENV
#
#
##
@env = `cat common/env.txt`;

push @final_text, "\n\.SH ENVIRONMENT \n.SP\n\n";

foreach(@env) {
    chomp;
  $row++;
  $line = $_;
  
  check_tag( $line, $row, "common/env.txt" );
  
  $line = mangletags($line);
  push @final_text, $line;
}


##
#
# END + AUTHORS
#
##

#push @final_text, "\n\\medskip\n\\textbf{AUTHORS}\n\\smallskip\n\nAlvise Dorigo (alvise.dorigo\@pd.infn.it)";

foreach(@final_text) {
  print;
}
