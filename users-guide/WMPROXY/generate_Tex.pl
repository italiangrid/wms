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

  $line =~ s/<<<SPACE>>>/\\s/g;
  $line =~ s/<<<VERB>>>/\\begin{verbatim}/g;
  $line =~ s/<<<ENDVERB>>>/\\end{verbatim}/g;
  $line =~ s/<<<NEWLINE>>>//g;
  $line =~ s/<<<ITEM>>>/\n\\item /g;
  $line =~ s/<<<ENUM>>>/\n\\begin{enumerate}\n/g;
  $line =~ s/<<<ENDENUM>>>/\n\\end{enumerate}\n/g;
  $line =~ s/<<<ITEMIZE>>>/\n\\begin{itemize}\n/g;
  $line =~ s/<<<ENDITEMIZE>>>/\n\\end{itemize}\n/g;
  $line =~ s/<<<INLINEVERB>>>/\\verb /g;
  $line =~ s/<<<ENDINLINEVERB>>>/ {}/g;
  $line =~ s/<<<USC>>>/\\_/g;
  $line =~ s/<<<DOLLAR>>>/\\\$/g;
  $line =~ s/<<<REF>>>/\\ref{/g;
  $line =~ s/<<<ENDREF>>>/}/g;
  $line =~ s/<<<PAGEREF>>>/\\pageref{/g;
  $line =~ s/<<<ENDPAGEREF>>>/}/g;
  $line =~ s/<<<BF>>>/\\textbf{/g;
  $line =~ s/<<<ENDBF>>>/}/g;
  $line =~ s/<<<ENDOPT>>>//g;
  return $line;
}

$CMDNAME = shift;
$row =0;
@final_text = ();

push @final_text, "\n\\subsection{$CMDNAME}\n\\label{$CMDNAME}\n\n\\medskip\n\\textbf{$CMDNAME}\n\\smallskip\n\n";

##
#
#
# GENERATE SYNOPSIS 
#
#
##

$sinopsis = `cat $CMDNAME/synopsis.txt`;
$sinopsis =~ s/_/\\_/g;
chomp($sinopsis);
check_tag($sinopsis, "1", "$CMDNAME/synopsis.txt");
$sinopsis = mangletags($sinopsis);

@option_verb = `cat $CMDNAME/options_verb.txt`;

push @final_text, "\n\\textbf{SYNOPSIS}\n\\smallskip\n\n\\textbf{$CMDNAME $sinopsis}\n\n\\begin{verbatim}\n";

foreach(@option_verb) {
  #chomp;
  $line = $_;
  check_tag( $line, "1", "$CMDNAME/options_verb.txt" );

  $line = mangletags( $line );
  push @final_text, $line;
}
push @final_text, "\\end{verbatim}\n";

##
#
#
# GENERATE DESCRIPTION 
#
#
##

push @final_text, "\n\\medskip\n\\textbf{DESCRIPTION}\n\\smallskip\n\n";

@desc_lines = `cat $CMDNAME/desc.txt`;
$verbatim = 0;
foreach(@desc_lines) {
  $row++;
  $line = $_;
  #chomp($line);
  
  check_tag( $line, $row, "$CMDNAME/desc.txt" );
  
#  $line =~ s/^\s+//;
#  $line =~ s/\s+$//;
  if($line =~ m/<<<VERB>>>/) {
    $verbatim = 1;
  }
  if($line =~ m/<<<ENDVERB>>>/) {
    $verbatim = 0;
  }
  if(($line =~ m/<<<VERB>>>/) && ($line =~ m/<<<ENDVERB>>>/) ) {
    die "Error at row $row: <<<VERB>>> and <<<ENDVERB>>> must be on different lines";
  }
  if($verbatim==0) {
    $line =~ s/--/-{}-/g;
    #$line =~ s/_/\\_/g;
    #$line =~ s/\$/\\\$/g;
  }
  
  $line = mangletags($line);
  push @final_text, $line;
}

##
#
#
# GENERATE OPTIONS
#
#
##
@option_description = `cat common/common_opts_1.txt $CMDNAME/opts.txt`;

push @final_text, "\n\n\\medskip\\textbf{OPTIONS}\\smallskip\n\n";

foreach(@option_description) {
  $row++;
  $line = $_;
  
  check_tag( $line, $row, "common/common_opts_1.txt or $CMDNAME/opts.txt" );
 
  if($line =~ m/<<<VERB>>>/) {
    $verbatim = 1;
  }
  if($line =~ m/<<<ENDVERB>>>/) {
    $verbatim = 0;
  }
  if($line =~ m/<<<VERB>>>/ && $line =~ m/<<<ENDVERB>>>/ ) {
    die "Error at row $row: <<<VERB>>> and <<<ENDVERB>>> must be on different lines";
  }
  if($verbatim==0) {
    $line =~ s/--/-{}-/g;
    #$line =~ s/_/\\_/g;
    #$line =~ s/\$/\/\$/g;
  }

  $line = mangletags($line);

  if($line =~ m/^<<<OPT>>>/) {
    $line =~ s/^<<<OPT>>>//;
    chomp($line);

    #print "*************** ADDING OPTION [$line]\n";

    push @final_text, "\n\n\\textbf{$line}\n";
    next;
  }
  
  push @final_text, $line;
}

##
#
#
# GENERATE EXAMPLES
#
#
##
@examples = `cat $CMDNAME/examples.txt`;
if($#examples > 0 ) {
  push @final_text, "\n\n\\medskip\n\\textbf{EXAMPLES}\n\\smallskip\n\n";

  foreach(@examples) 
  {
    $row++;
    
    $line = $_;
  
    check_tag( $line, $row, "$CMDNAME/examples.txt" );
  
    if($line =~ m/<<<VERB>>>/) {
      $verbatim = 1;
    }
    if($line =~ m/<<<ENDVERB>>>/) {
      $verbatim = 0;
    }
    if($line =~ m/<<<VERB>>>/ && $line =~ m/<<<ENDVERB>>>/ ) {
      die "Error at row $row: <<<VERB>>> and <<<ENDVERB>>> must be on different lines";
    }
    if($verbatim==0) {
      $line =~ s/--/-{}-/g;
    }
  
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

push @final_text, "\n\n\\medskip\n\\textbf{FILES}\n\\smallskip\n\n";


foreach(@files) {
  $row++;
  #chomp;
  $line = $_;
  
  check_tag( $line, $row, "$CMDNAME/files.txt" );
  
  if($line =~ m/<<<VERB>>>/) {
    $verbatim = 1;
    #next;
  }
  if($line =~ m/<<<ENDVERB>>>/) {
    $verbatim = 0;
    #next;
  }
  if($line =~ m/<<<VERB>>>/ && $line =~ m/<<<ENDVERB>>>/ ) {
    die "Error at row $row: <<<VERB>>> and <<<ENDVERB>>> must be on different lines";
  }
  if($verbatim==0) {
    $line =~ s/--/-{}-/g;
    #$line =~ s/_/\\_/g;
    #$line =~ s/\$/\\\$/g;
  }

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

push @final_text, "\n\n\\medskip\n\\textbf{ENVIRONMENT}\n\\smallskip\n\n";

foreach(@env) {
  $row++;
  #chomp;
  $line = $_;
  
  check_tag( $line, $row, "common/env.txt" );
  
  if($line =~ m/<<<VERB>>>/) {
    $verbatim = 1;
    #next;
  }
  if($line =~ m/<<<ENDVERB>>>/) {
    $verbatim = 0;
    #next;
  }
  if($line =~ m/<<<VERB>>>/ && $line =~ m/<<<ENDVERB>>>/ ) {
    die "Error at row $row: <<<VERB>>> and <<<ENDVERB>>> must be on different lines";
  }
#  if($line =~ m/^<<<VERB>>>/) {
  if($verbatim==0 && $inlineverb == 0) {
    $line =~ s/--/-{}-/g;
    #$line =~ s/_/\\_/g;
    #$line =~ s/\$/\\\$/g;
  }

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
