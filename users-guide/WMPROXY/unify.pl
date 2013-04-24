#!/usr/bin/perl

@source_lines = <>;
$INPUT = 0;
foreach (@source_lines) {
  chomp;
  $line = $_;
  $line =~ s/^\s+//;
  $line =~ s/\s+$//;
  if($line =~ m/^%/) {next;}
  if($line =~ m/\\input/)
  {
    #print "FILENAME TO LOAD: $line\n";
    $line =~ s/\\input//;
    $line =~ s/{//;
    $line =~ s/}//;
    #print "FILENAME TO LOAD: $line\n";
    
    if ($line !~ /\.tex$/) {
      $line = "$line.tex";
    }
    
    @inputfile = `cat $line`;
    foreach (@inputfile) {
      chomp;
      $_line = $_;
      $_line =~ s/^\s+//;
      $_line =~ s/\s+$//;
      if($_line =~ m/^%/) {next;}
      push @target, $_line;
    }
    $INPUT = 1;
    next;
  }
#  print "$line\n";
  push @target, $line;
  next;
}

foreach (@target) {
  print STDOUT "$_\n";
}

if($INPUT == 1) {
  print STDERR "Files MERGED\n";
} else {
  print STDERR "There wasn't any file to merge\n";
}

