#!/usr/bin/perl

my $definitionFile = "";
my $version = "1.6";
my $verbose = 0;
my $style = 0; # "narrow" style, 1 for "wide" style...

sub displayHelp {
    print "Unknown parameter or insufficient number of parameters.\n" if( $_[0] );

    print "Usage: $0 [options] <definitions file> [fileopt <templatefile>]\n";
    print "Where options could be:\n";
    print "\t-h, --help\n";
    print "\t\tDisplay this help.\n";
    print "\t-v, --verbose\n";
    print "\t\tVerbose operations.\n";
    print "While fileopt could be:\n";
    print "\t-c, --code\n";
    print "\t\tGenerates a code file using the definitions read in the\n";
    print "\t\tdefinitions file and putting the generated code in the\n";
    print "\t\ttemplate file. If the template file ends with \".G\", this\n";
    print "\t\tsuffix is removed, else the \".cpp\" suffix is added to the\n";
    print "\t\tname.\n";
    print "\t-H, --header\n";
    print "\t\tWorks like the -c options, but generates an header file.\n";

    exit( $_[0] );
}

sub writeFile {
    my $file = shift( @_ );

    open( FILE, ">$file" ) || die "Cannot open output file \"$file\".\n";
    print FILE @_;
    close( FILE );

    print "File \"$file\" modified.\n" if( $verbose );
}

sub checkAndWriteFile {
    my $file = shift( @_ );
    my @NewCode = @_;

    if( open(OLDCODE, "<$file") ) {
	my @OldCode = <OLDCODE>;
	close( OLDCODE );

	my $equal = 1;

	if( $equal = ($#OldCode == $#NewCode) ) {
	    for( my $i = 0; $i < @OldCode; $i++ ) {
		if( $OldCode[$i] ne $NewCode[$i] ) {
		    $equal = 0;
		    last;
		}
	    }
	}

	if( $equal ) { print "File \"$file\" unmodified.\n" if( $verbose ); }
	else { &writeFile( $file, @NewCode ); }
    }
    else { &writeFile( $file, @NewCode ); }
}

sub compactSpaces {
    my $result = $_[0];

    $result =~ tr/ \n\t/ /s;
    $result =~ s/^\s+(.*)/$1/;
    $result =~ s/^(.*) $/$1/;
    $result =~ s/ ?,/,/g;

    $result =~ s/, /,/g if( $_[1] );

    return( $result );
}

sub compactOperators {
    my $thing = $_[0];

    if( ref($thing) eq "ARRAY" ) {
	foreach my $val ( @{$thing} ) {
	    $val =~ s/\s+\*/\*/g;
	    $val =~ s/\s+\&/\&/g;
	}
    }
    else {
	$thing =~ s/\s+\*/\*/g;
	$thing =~ s/\s+\&/\&/g;
    }

    return( $thing )
}

sub tabs {
    my $tabs = "";

    for( my $i = 0; $i < $_[0]; $i++ ) { $tabs .= " "; }

    return( $tabs );
}

sub studyBody {
    my $oldbody = $_[0];
    my $tabpos = 0; my $body = "";

    while( $oldbody ne "" ) {
	if( $oldbody =~ /^\s*\{ (.*)$/ ) {
	    $body .= &tabs( $tabpos ) . "{\n";
	    $oldbody = $1; $tabpos += 2;
	}
	elsif( $oldbody =~ /^\s*([^\{\}\;]+)\;(.*)$/ ) {
	    $body .= &tabs( $tabpos ) . "$1;\n";
	    $oldbody = $2; 
	}
	elsif( $oldbody =~ /^\s*([^\{\}\;]+)(.*)$/ ) {
	    $body .= &tabs( $tabpos ) . "$1\n";
	    $oldbody = $2;
	}
	elsif( $oldbody =~ /^\s*\}(.*)$/ ) {
	    $tabpos -= 2;
	    $body .= &tabs( $tabpos ) . "}\n";
	    $oldbody = $1;
	}
    }

    $body =~ s/ ;/;/g;

    return( $body );
}

sub loadClasses {
    my $file = $_[0];
    my $state = 0; my $current = my $line = my $buffer = "";
    my %Classes = ();
    my $closed = my $opened = 0;

    print "Loading classes definitions from file \"$file\".\n" if( $verbose );

    open( FILE, "<$file" ) || die "Cannot open definitions file \"$file\"\n";
    my @Lines = <FILE>;
    close( FILE );

    while( $line = shift(@Lines) ) {
	next if( $line =~ /^(([\s\n]*)|(\#.*))$/ ); # Remove spaces and empty lines...

	if( $state == 0 ) { # reading the generic part...
	    if( $line =~ /^\s*class\s*([a-zA-Z\_][a-zA-Z0-9\_]*)\s*\{\s*$/ ) { # ...but found a class definition...
		die "Class name \"$1\" is reserved.\n" if( $1 eq "Extern" );
		die "Class \"$1\" already defined.\n" if( $Classes{$1} );

		my @lines = (); 

		$current = $1; $state = 1; $buffer = "{";
		$Classes{$1} = \@lines;

		print "\tFound definition for class \"$1\"\n" if( $verbose );
	    }
	    elsif( $line =~ /^\s*\%style\s+([a-z]*)\s*$/ ) {
		if( $1 eq "wide" ) { $style = 1; }
		elsif( $1 eq "narrow" ) { $style = 0; }
		else { die "Unknown style \"$1\" found.\n"; }
	    }
	    else { 
		unless( $Classes{"0"} ) {
		    my @lines = ( $line );

		    $Classes{"0"} = \@lines;
		}
		else { push( @{$Classes{"0"}}, $line ); }
	    }
	}
	elsif( $state == 1 ) { # Reading a class definition...
	    if( $line =~ /^\s*class\s*([a-zA-Z\_][a-zA-Z0-9\_]*)(.*)$/ ) { 
		die "Found beginning of class \"$1\" while scanning class \"$current\".\n"; 
	    }
	    elsif( $line =~ /^\s*\}\s*$/ ) {
		$opened = ($buffer =~ tr/\{/\{/);
		$closed = ($buffer =~ tr/\}/\}/) + 1;

		if( $opened == $closed ) { 
		    $state = 0;

		    print "\tEnd of class \"$current\"\n" if( $verbose );
		}
		else {
		    push( @{$Classes{$current}}, $line );
		    $buffer .= $line;
		}
	    }
	    else {
		push( @{$Classes{$current}}, $line );
		$buffer .= $line;
	    }
	}
    }

    return( \%Classes );
}

sub createClass {
    my @Functions = (); my @CFunctions = (); my @Parameters = ();
    my @Sections = (); my %Defaults = ();
    my %Wrapper = ( "contexttype" => "void *",
		    "contextfunc" => "",
		    "context" => "",
		    "prefix" => "" );
    my %Class = ( "Functions"  => \@Functions,
		  "CFunctions" => \@CFunctions,
		  "Parameters" => \@Parameters,
		  "Sections"   => \@Sections,
		  "Defaults"   => \%Defaults,
		  "Wrapper"    => \%Wrapper,
		  "Name"       => $_[0] );

    return( %Class );
}

sub parseClass {
    (my $name, my $Classes) = @_;
    my $line = my $buffer = my $car = "";
    my $section = "0";
    my $state = my $function = my $parameter = my $wrapper = my $ctemplate = 0;
    my @ClassLines = @{$Classes->{$name}};
    my %Class = &createClass( $name );

    while( $line = shift(@ClassLines) ) {
	$buffer .= " ";

	while( $line !~ /^\s*$/ ) {
	    if( $state == 0 ) { # Outside any definition...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^\s*template(.*)$/ ) {
		    $state = 1;
		    $line = $1;

		    my %Temp = ();
		    $function = \%Temp;
		}
		elsif( $line =~ /^\s*ctemplate(.*)$/ ) {
		    $state = $ctemplate = 1;
		    $line = $1;

		    my %Temp = ();
		    $function = \%Temp;
		}
		elsif( $line =~ /^\s*parameter(.*)$/ ) {
		    $state = 9;
		    $line = $1;

		    my %Temp = ();
		    $parameter = \%Temp;
		}
		elsif( $line =~ /^\s*section(.*)$/ ) {
		    $state = 12;
		    $line = $1;
		}
		elsif( $line =~ /^\s*wrapper(.*)$/ ) {
		    $state = 15;
		    $line = $1;
		}
		elsif( $line =~ /^\s*\}(.*)$/ ) {
		    die "Unbalanced right bracket at \"$line\".\n" if( ($section eq "0") && !$wrapper );

		    $line = $1;
		    $section = "0";
		}
		elsif( $line =~ /^\s*copy\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\.?(G?[a-z]*)\s*\;(.*)$/ ) {
		    my $section = $2; my $class = $1;

		    $class = "0" if( $class eq "Extern" );

		    if( $section ) {
			if( $section =~ /^[a-z]+$/ ) { $class .= ".$section"; }
			elsif( $section eq "Generic" ) { $class .= ".0"; }
			else { die "Wrong class section specification at \"$line\"\n"; }
		    }
		    else { $class .= ".0"; }

		    unless( $Class{"copy"} ) {
			my @copy = ( $class );
			$Class{"copy"} = \@copy;
		    }
		    else { push( @{$Class{"copy"}}, $class ); }

		    $line = $3;
		}
		elsif( $line =~ /^\s*defaultvalue\s+(.*)\s*=\s*(.*)\;(.*)$/ ) {
		    my $type = &compactSpaces( $1, 0 );
		    my $value = &compactSpaces( $2, 0 );

		    $Class{"Defaults"}->{$type} = $value;

		    $line = $3;
		}
		else { die "Unknown keyword found at \"$line\"\n"; }
	    }
	    elsif( $state == 1 ) { # Start of the template types...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^\s*\((.*)$/ ) {
		    $state = 2;
		    $line = $1; $buffer = "";
		}
		else { die "Parse error before \"$line\"\n"; }
	    }
	    elsif( $state == 2 ) { # Reading the templates...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\)]+)\)(.*)$/ ) {
		    $buffer .= $1; $line = $2;
		    my @types = split( /,/, &compactSpaces($buffer, 1) );

		    if( $ctemplate ) {
			my @mapped = ();

			foreach my $type ( @types ) {
			    if( $type =~ /^\s*(.*)\s*->\s*(.*)\s*$/ ) {
				my $origtype = &compactSpaces( $1 );
				my $newtype = &compactSpaces( $2 );

				if( $function->{"mapping"} ) { $function->{"mapping"}->{$origtype} = $newtype; }
				else {
				    my %mapping = ( $origtype => $newtype );

				    $function->{"mapping"} = \%mapping;
				}

				push( @mapped, $origtype );
			    }
			    else { push( @mapped, $type ); }
			}

			$function->{"types"} = \@mapped;
		    }
		    else { $function->{"types"} = \@types; }

		    $function->{"types"} = &compactOperators( $function->{"types"} );

		    $state = 3; $buffer = "";
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 3 ) { # Read function name...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\(]*)(\(.*)$/ ) {
		    $buffer .= $1; $line = $2;
		    $function->{"name"} = &compactSpaces( $buffer, 0 );

		    $state = 4; $buffer = "";
		}
		else { 
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 4 ) { # Read parameters list...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\)]+\))(.*)$/ ) {
		    $buffer .= $1; $line = $2;
		    $buffer =~ s/\(/ \( /g; $buffer =~ s/\)/ \) /g;
		    $function->{"parameters"} = &compactSpaces( $buffer );

		    $state = 5; $buffer = "";
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 5 ) { # Search the beginning of the body of the comment or of the verbatim part...
		if( $line =~ /^\s*(\{.*)$/ ) {
		    $state = 6;
		    $line = $1;
		}
		elsif( $line =~ /^\s*\[(.*)$/ ) {
		    $state = 7;
		    $line = $1;
		}
		elsif( $line =~ /^\s*\<(.*)$/ ) {
		    $state = 13;
		    $line = $1;
		}
		else { die "Syntax error after function parameters, at \"$line\".\n"; }
	    }
	    elsif( $state == 6 ) { # Reading the function body...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\}]*\})(.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    my $opened = $buffer =~ tr/\{/\{/; my $closed = $buffer =~ tr/\}/\}/;

		    if( $opened == $closed ) {
			$buffer =~ s/ *\;/; /g;
			$buffer =~ s/\(/ \( /g; $buffer =~ s/\)/ \) /g;
			$buffer =~ s/\{/ \{ /g; $buffer =~ s/\}/ \} /g;
			$buffer = &compactSpaces( $buffer, 0 );
			$function->{"body"} = &studyBody( $buffer );

			$state = 0; $buffer = "";

			$function->{"section"} = "$name.$section";
			$function->{"class"} = \%Class;

			if( $ctemplate ) {
			    push( @{$Class{"CFunctions"}}, $function );
			    $ctemplate = 0;
			}
			else { push( @{$Class{"Functions"}}, $function ); }
		    }
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 7 ) { # Reading verbatim comment...
		if( $line =~ /^([^\]]*)\](.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    $function->{"comment"} = $buffer;

		    $state = 8; $buffer = "";
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 8 ) { # Search only the beginning of the function body...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^\s*(\{.*)$/ ) {
		    $state = 6;
		    $line = $1;
		}
		else { die "Syntax error expecting function body at \"$line\".\n"; }
	    }
	    elsif( $state == 9 ) { # Reading the parameter type...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\(]+)\((.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    $parameter->{"type"} = &compactSpaces( $buffer );
		    $parameter->{"type"} = &compactOperators( $parameter->{"type"} );

		    $state = 10; $buffer = "";
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 10 ) { # Reading the parameter names...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\)]+)\)(.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    my @names = split( /,/, &compactSpaces($buffer, 1) );
		    my $defval = $Class{"Defaults"}->{$parameter->{"type"}};
		    my $tofunction = "";

		    if( (@names == 0) || (@names > 2) ) {
			die "Insufficient number or too many tokens found for parameter at \"$line\"\n";
		    }

		    my @parts = split( /=/, $names[0] );
		    if( @parts == 2 ) {
			$names[0] = &compactSpaces( $parts[0] );
			$defval = &compactSpaces( $parts[1] );
		    }

		    @parts = split( /->/, $names[0] );
		    if( @parts == 2 ) {
			$names[0] = &compactSpaces( $parts[0] );
			$tofunction = &compactSpaces( $parts[1] );
		    }

		    my @equal = grep {
			($names[0] eq $_->{"names"}->[0]) || (($names[1] ne "") && ($names[1] eq $_->{"names"}->[1]))
			} @{$Class{"Parameters"}};
		    die "Duplicated parameter \"" . $names[0] . "\"." if( @equal );

		    $parameter->{"names"} = \@names;
		    $parameter->{"default"} = $defval if( defined($defval) );
		    $parameter->{"functionname"} = $tofunction if( defined($tofunction) );

		    $state = 11; $buffer = "";
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 11 ) { # Reading the parameter scopes...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\;]*)\;(.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    $buffer = &compactSpaces( $buffer, 1 );

		    unless( $buffer eq "" ) {
			my @scopes = split( /,/, $buffer );
			$parameter->{"scopes"} = \@scopes;
		    }
		    else {
			my @scopes = ( "class" );
			$parameter->{"scopes"} = \@scopes;
		    }

		    $state = 0; $buffer = "";

		    $parameter->{"section"} = "$name.$section";
		    $parameter->{"class"} = \%Class;
		    push( @{$Class{"Parameters"}}, $parameter );
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 12 ) { # Reading class section...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\{]*)\{(.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    $buffer = &compactSpaces( $buffer );
		    if( ($buffer !~ /^[a-z]+$/) || ($buffer eq "functions") || ($buffer eq "parameters") ) {
			die "Illegal class section specification \"$buffer\".\n" ;
		    }

		    push( @{$Class{"Sections"}}, $buffer );

		    $section = $buffer; $buffer = "";
		    $state = 0;

		    print "\tReading section \"$section\" of class \"" . ( ($name eq "0") ? "generic" : $name ) . "\".\n" if( $verbose );
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 13 ) { # Reading verbatim part after function parameters...
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^([^\>]*)\>(.*)$/ ) {
		    $buffer .= $1; $line = $2;

		    $function->{"verbatim"} = " " . &compactSpaces( $buffer, 0 );

		    $state = 14; $buffer = "";
		}
		else {
		    $buffer .= $line;
		    $line = "";
		}
	    }
	    elsif( $state == 14 ) { # Search the beginning of the body or of the comment
		if( $line =~ /^\s*(\{.*)$/ ) {
		    $state = 6;
		    $line = $1;
		}
		elsif( $line =~ /^\s*\[(.*)$/ ) {
		    $state = 7;
		    $line = $1;
		}
		else { die "Syntax error after function verbatim, at \"$line\"\n"; }
	    }
	    elsif( $state == 15 ) { # Reading wrapper in current section
		$car = chop( $line );
		$line .= $car if( $car ne "\n" );

		if( $line =~ /^\s*\{(.*)$/ ) {
		    $wrapper = $Class{"Wrapper"};

		    print "\tFound C wrapper definitions...\n" if( $verbose );

		    $line = $1;
		    $state = 16;
		}
		elsif( $line !~ /^\s*$/ ) {
		    die "Dust in definition file while scanning wrapper for class \"" 
			. ( ($name eq "0") ? "generic" : $name ) . "\"\n";
		}
		else { $line = ""; }
	    }
	    elsif( $state == 16 ) {
		if( $line =~ /^\s*context\s*=\s*(.*);(.*)$/ ) {
		    my $context = &compactSpaces( $1 );
		    $wrapper->{"context"} = $context;

		    $line = $2;

		    print "\t\tC context = $context\n" if( $verbose );
		}
		elsif( $line =~ /^\s*prefix\s*=\s*([a-zA-Z0-9_]+)\s*;(.*)$/ ) {
		    my $prefix = &compactSpaces( $1 );
		    $wrapper->{"prefix"} = $prefix;

		    $line = $2;

		    print "\t\tWrapper prefix = $prefix\n" if( $verbose );
		}
		elsif( $line =~ /^\s*contexttype\s*=\s*(.*)\s*;(.*)$/ ) {
		    my $contexttype = &compactSpaces( $1 );
		    $wrapper->{"contexttype"} = $contexttype;

		    $line = $2;

		    print "\t\tContext type = $contexttype\n" if( $verbose );
		}
		elsif( $line =~ /^\s*contextfunc\s*=\s*(.*)\s*;(.*)$/ ) {
		    my $contextfunc = &compactSpaces( $1 );
		    $wrapper->{"contextfunc"} = $contextfunc;

		    $line = $2;

		    print "\t\tContext function = $contextfunc\n" if( $verbose );
		}
		elsif( $line =~ /^\s*\}(.*)$/ ) {
		    $line = $1;

		    $Class{"Wrapper"} = $wrapper;

		    $state = $wrapper = 0;
		}
		else {
		    die "Dust in definition file while scanning wrapper for class \""
			. ( ($name eq "0") ? "generic" : $name ) . "\"\n";
		}
	    }
	}
    }

    die "End of file found while parser in state $state.\n" if( $state );

    return( \%Class );
}

sub rearrangeParameters {
    (my $Classes) = @_;

    foreach my $name ( keys %{$Classes} ) {
	print "Rearranging parameters in class \"$name\"\n" if( $verbose );

	foreach my $param ( @{$Classes->{$name}->{"Parameters"}} ) {
	    next if( $param->{"class"}->{"Name"} ne $name );

	    my @sections = ( $param->{"section"} );
	    $param->{"sections"} = \@sections;

	    print "\tChecking parameter \"" . $param->{"names"}->[0] . "\" (" . $param->{"type"} . ")\n" if( $verbose );

	    if( @{$param->{"scopes"}} ) {
		foreach my $scope ( @{$param->{"scopes"}} ) {
		    print "\t\tChecking scope \"$scope\".\n" if( $verbose );

		    if( $scope eq "class" ) { next; }
		    elsif( $scope =~ /^section ((Generic)|([a-z]+))$/ ) {
			my $partname = ( ($1 eq "Generic") ? "0" : $1 );
			my $section = "$name.$partname";

			push( @{$param->{"sections"}}, $section ) unless( grep(/$section/, @{$param->{"sections"}}) );
		    }
		    elsif( $scope =~ /^class ([a-zA-Z\_][a-zA-Z0-9\_]*)\.?((Generic)|([a-z]*))$/ ) {
			my $classname = $1;
			my $partname = ( (!$2 || ($2 eq "Generic")) ? "0" : $2 );
			my $section = "$classname.$partname";

			if( $classname ne $name ) {
			    die "Scope of parameter \"" . $param->{"names"}->[0] 
				. "\" requested for unexistent class \"$classname\"\n" unless( $Classes->{$classname} );

			    push( @{$Classes->{$classname}->{"Parameters"}}, $param );
			}

			push( @{$param->{"sections"}}, $section ) unless( grep(/$section/, @{$param->{"sections"}}) );
		    }
		    elsif( $scope =~ /extern\.?((Generic)|([a-z]*))$/ ) {
			my $classname = "0";
			my $partname = ( (!$1 || ($1 eq "Generic")) ? "0" : $1 );
			my $section = "$classname.$partname";

			if( $classname ne $name ) {
			    unless( $Classes->{$classname} ) {
				my %Class = &createClass( $classname );
				$Classes->{$classname} = \%Class;
			    }

			    push( @{$Classes->{$classname}->{"Parameters"}}, $param );
			}

			push( @{$param->{"sections"}}, $section ) if( !grep(/$section/, @{$param->{"sections"}}) );
		    }
		    elsif( $scope eq "remove" ) {
			my @sections = grep( !/$name\.0/, @{$param->{"sections"}} );

			$param->{"sections"} = \@sections;
		    }
		    else {
			die "Scope \"$scope\" unknown in this context.\n";
		    }
		}
	    }
	}
    }
}

sub mergeWrappers {
    (my $old, my $new) = @_;

    $old->{"contexttype"} = $new->{"contexttype"} if( $new->{"contexttype"} ne "void *" );
    $old->{"contextfunc"} = $new->{"contextfunc"} if( $new->{"contextfunc"} ne "" );
    $old->{"context"} = $new->{"context"} if( $new->{"context"} ne "" );
    $old->{"prefix"} = $new->{"prefix"} if( $new->{"prefix"} ne "" );

    return( $old );
}

sub copySections {
    (my $Classes) = @_;

    foreach my $name ( keys %{$Classes} ) {
	if( $Classes->{$name}->{"copy"} ) {
	    print "Checking copied sections in class \"$name\"\n" if( $verbose );

	    foreach my $copy ( @{$Classes->{$name}->{"copy"}} ) {
		$copy =~ /^([a-zA-Z0-9\_]+)\.?((0)|([a-z]*))$/;
		die "Found request for unexistent class \"$1\"\n" unless( $Classes->{$1} );

		if( $2 ne "0" ) {
		    die "Found request for unexistent class section \"$1.$2\".\n" unless( grep(/$2/, @{$Classes->{$1}->{"Sections"}}) );
		}
		
		my @parameters = grep { $_->{"section"} eq "$1.$2" } @{$Classes->{$1}->{"Parameters"}};
		my @functions = grep { $_->{"section"} eq "$1.$2" } @{$Classes->{$1}->{"Functions"}};
		my @cfunctions = grep { $_->{"section"} eq "$1.$2" } @{$Classes->{$1}->{"CFunctions"}};
		my @newpars; my @newfuncs; my @newcfuncs;

		foreach my $param ( @parameters ) {
		    my %newpar = %{$param};
		    $newpar{"section"} = "$name.$2";
		    $newpar{"class"} = $Classes->{$name};

		    if( !defined($newpar{"default"}) && defined($Classes->{$name}->{"Defaults"}->{$newpar{"type"}}) ) {
			$newpar{"default"} = $Classes->{$name}->{"Defaults"}->{$newpar{"type"}};
		    }

		    push( @newpars, \%newpar );
		}
		foreach my $func ( @functions ) {
		    my %newfunc = %{$func};
		    $newfunc{"section"} = "$name.$2";

		    push( @newfuncs, \%newfunc );
		}
		foreach my $func ( @cfunctions ) {
		    my %newfunc = %{$func};
		    $newfunc{"section"} = "$name.$2";

		    push( @newcfuncs, \%newfunc );
		}

		print "\tMerging class \"$1" . ( $2 ? ".$2" : "" ) . "\" (" . @newpars . ", " . @newfuncs . ").\n" if( $verbose );
		$Classes->{$name}->{"Wrapper"} = &mergeWrappers( $Classes->{$name}->{"Wrapper"}, $Classes->{$1}->{"Wrapper"} );

		push( @{$Classes->{$name}->{"Functions"}}, @newfuncs );
		push( @{$Classes->{$name}->{"Parameters"}}, @newpars );
		push( @{$Classes->{$name}->{"CFunctions"}}, @newcfuncs );
	    }
	}
    }
}

sub loadDefinitions {
    my %Definitions = ();
    my $Classes = &loadClasses( $_[0] );
    $definitionFile = $_[0];

    foreach my $key ( keys %{$Classes} ) {
	print "Parsing definition of class \"" . ( ($key eq "0") ? "generic" : $key ) . "\".\n" if( $verbose );
	$Definitions{$key} = &parseClass( $key, $Classes );
    }

    &copySections( \%Definitions );
    &rearrangeParameters( \%Definitions );

    return( \%Definitions );
}

sub createSuffix {
    my @splitted = split( /([A-Z])/, (($_[1] eq "") ? $_[0] : $_[1]) );
    my @reversed = reverse grep { $_ } @splitted;
    my $result = "";
    my $upper = 0; my $last = "";

    foreach my $car ( @reversed ) {
	my $copy = $last;

	if( ($copy =~ tr/[A-Z]/[a-z]/) && ($car =~ /[A-Z]/) ) {
	    $result = $copy . $result;
	    $last = $car;
	}
	else {
	    $result = $last . $result;
	    $last = $car;
	}
    }
    $result = $last . $result;

    if( $result =~ /^([a-z])(.*)$/ ) {
	my $tmp = $1; my $aft = $2;

	$tmp =~ y/[a-z]/[A-Z]/;

	$result = $tmp . $aft;
    }

    $result =~ s/-/\_/g;
    $result =~ s/(.*)_([A-Z])(.*)/$1$2$3/;

    @splitted = split( /([A-Z])/, $result );
    $result = "";

    foreach my $spl ( @splitted ) {
	if( $spl =~ y/[A-Z]/[a-z]/ ) { $result .= "_$spl"; }
	else { $result .= "$spl"; }
    }

    $result =~ s/\_\_/\_/g;

    return( $result );
}

sub splitNewlinesBuffer {
    (my $buffer, my $tabs) = @_;
    my @Lines = ();
    my @temp = split( /\n/, $buffer );

    foreach my $line ( @temp ) { push( @Lines, &tabs($tabs) . $line. "\n" ); }

    return( \@Lines );
}

sub parseValueAndType {
    my $parameter = shift( @_ ); my $parname = shift( @_ ); my $funcsuffix = shift( @_ ); my $completename = shift( @_ );
    my $class = shift( @_ ); my $mapping = shift( @_ );
    my @Lines = grep { !/^ +\n$/ } @_;
    my $type = $parameter->{"type"};
    my $default = $parameter->{"default"};
    my $contexttype = my $context = my $contextfunc = my $classprefix = "";

    $type = $mapping->{$type} if( $mapping && $mapping->{$type} );

    if( $class ) {
	$contexttype = $class->{"Wrapper"}->{"contexttype"};
	$context = $class->{"Wrapper"}->{"context"};
	$contextfunc = $class->{"Wrapper"}->{"contextfunc"};
	$classprefix = $class->{"Wrapper"}->{"prefix"};
    }

    if( @Lines != 0 ) {
	foreach my $line ( @Lines ) {
	    if( $class ) {
		$line =~ s/\@CLASSPREFIX\@/$classprefix/g;
		$line =~ s/\@CLASSCONTEXT\@/$context/g;
		$line =~ s/\@CONTEXTTYPE\@/$contexttype/g;
		$line =~ s/\@CONTEXTFUNC\@/$contextfunc/g;
	    }
	    $line =~ s/\@FUNCNAME\@/$completename/g;
	    $line =~ s/\@ATTRTYPE\@/$type/g;
	    $line =~ s/\@ATTRNAME\@/$parname/g;
	    $line =~ s/\@ATTRSUFFIX\@/$funcsuffix/g;
	    if( $line =~ /\@ATTRDEF\@/ ) {
		if( defined($default) ) {
		    $line =~ s/\@ATTRDEF\@/$default/g;
		}
		else {
		    die "Default value selected for type \"$type\" of parameter $parname which has undefined default.\n";
		}
	    }
	    $line =~ s/ \(/\(/g;
	    $line =~ s/\) ([\;\{\}])/\)$1/g;
	    $line =~ s/\(\s+\)/\(\)/g;
	    $line =~ s/([^ ])  ([^ ])/$1 $2/g;

	    if( $style ) {
		$line =~ s/ ?\* ?/ \*/g;
		$line =~ s/ \& / \&/g;
		$line =~ s/([^\&])\& /$1\&/g;
#		$line =~ s/ ?\& ?/ \&/g;
		$line =~ s/\(\(/\( \(/g;
		$line =~ s/\)\)/\) \)/g;
	    }
	    else {
		$line =~ s/\( /\(/g;
		$line =~ s/ \)/\)/g;
		$line =~ s/ ?\* ?/\* /g;
#		$line =~ s/ ?\& ?/\& /g;
		$line =~ s/ \&([^\&])/\& $1/g;
		$line =~ s/ \& /\& /g;
	    }

	    $line =~ s/\/ \*/\/\*/g;
	    $line =~ s/\* \//\*\//g;
	    $line =~ s/\/\*([^ ])/\/\* $1/g;
	    $line =~ s/([^ ])\*\//$1 \*\//g;
	}
    }

    return( \@Lines );
}

sub createFunctionPrototype {
    (my $function, my $parameter, my $tabs) = @_;
    my @Lines = (); my $line = "";
    my $funcsuffix = &createSuffix( $parameter->{"names"}->[0], $parameter->{"functionname"} );
    my $fullparname = ""; my $funcname = $function->{"name"};
    my $completename;

    unless( $parameter->{"names"}->[1] ) { $fullparname = "\"" . $parameter->{"names"}->[0] . "\""; }
    elsif( $parameter->{"names"}->[1] =~ /^ref\[([^\]]+)\]$/ ) { $fullparname = &compactSpaces( $1 ); }
    else {
	if( ($parameter->{"class"}->{"Name"} ne "0") && ($parameter->{"class"}->{"Name"} ne $function->{"class"}->{"Name"}) ) {
	    $fullparname = $parameter->{"class"}->{"Name"} . "::" . $parameter->{"names"}->[1];
	}
	else { $fullparname = $parameter->{"names"}->[1]; }
    }

    if( $function->{"comment"} ne "" ) {
	my $lines = &splitNewlinesBuffer( $function->{"comment"}, 0 );
	push( @Lines, @{$lines} );
    }

    if( $funcname !~ /^(.*)\@\.\@/ ) {
	$completename = $funcname . $funcsuffix;
    }
    else {
	my $loctype = "$1 ";
	$funcsuffix =~ /^\_(.*)$/;
	$completename = $loctype . $1;
    }

    $line = &tabs($tabs) . $completename . $function->{"parameters"} . $function->{"verbatim"}; 

    my @names = split( / /, $completename );
    $completename = $names[@names - 1];

    print "\t\tCreating function $completename\n" if( $verbose );

    if( $function->{"name"} =~ /^inline\s(.*)$/ ) {
	$line .= "\n"; push( @Lines, $line );

	my $body = &splitNewlinesBuffer( $function->{"body"}, $tabs );
	push( @Lines, @{$body} );
    }
    else {
	$line .= ";\n";
	push( @Lines, $line );
    }

    push( @Lines, "\n" );

    return( &parseValueAndType($parameter, $fullparname, $funcsuffix, $completename, 0, 0, @Lines) );
}

sub createCFunctionPrototype {
    (my $Class, my $function, my $parameter ) = @_;
    my @Lines = (); my $line = "";
    my $paramname = "\"" . $parameter->{"names"}->[0] . "\"";
    my $paramsuffix = &createSuffix( $parameter->{"names"}->[0], $parameter->{"functionname"} );
    my $funcname = $function->{"name"};
    my $completename = "";

    if( $function->{"comment"} ne "" ) {
	my $lines = &splitNewlinesBuffer( $function->{"comment"}, 0 );
	push( @Lines, @{$lines} );
    }

    if( $funcname =~ /^(.*)\@\.\@(.*)$/ ) {
	my $before = $1; my $after = $2;
	$paramsuffix =~ /^\_(.*)$/;
	$completename = $1 . $after;

	$line = $before . $completename . $function->{"parameters"} . $function->{"verbatim"};
    }
    else { $line = $funcname . $paramsuffix . $function->{"parameters"} . $function->{"verbatim"};; }

    my @names = split( / /, $completename );
    $completename = $names[@names - 1];

    print "\t\tCreating C wrapper prototype $completename\n" if( $verbose );

    push( @Lines, $line . ";\n" );
    push( @Lines, "\n" );

    return( &parseValueAndType($parameter, $paramname, $paramsuffix, $completename, $Class, $function->{"mapping"}, @Lines) );
}

sub createFunctionCode {
    (my $classprefix, my $function, my $parameter) = @_;
    my @Lines = (); my $line = "";
    my $funcsuffix = &createSuffix( $parameter->{"names"}->[0], $parameter->{"functionname"} );
    my $fullparname = ""; my $completename = "";

    unless( $parameter->{"names"}->[1] ) { $fullparname = "\"" . $parameter->{"names"}->[0] . "\""; }
    elsif( $parameter->{"names"}->[1] =~ /^ref\[([^\]]+)\]$/ ) { $fullparname = &compactSpaces( $1 ); }
    else {
	if( ($parameter->{"class"}->{"Name"} ne "0") && ($parameter->{"class"}->{"Name"} ne $function->{"class"}->{"Name"}) ) {
	    $fullparname = $parameter->{"class"}->{"Name"} . "::" . $parameter->{"names"}->[1];
	}
	else { $fullparname = $parameter->{"names"}->[1]; }
    }

    if( $function->{"name"} !~ /^inline\s(.*)$/ ) {
	my $funcname = $function->{"name"};

	$funcname =~ s/\sstatic\s//;
	$funcname =~ s/\sextern\s//;

	if( $style ) { $funcname =~ s/^(.*) ([\*\&]?)\s*([a-zA-Z\_]+)$/$1 $2$classprefix$3/; }
	else { $funcname =~ s/^(.*) ([\*\&]?)\s*([a-zA-Z\_]+)$/$1$2 $classprefix$3/; }

	if( $funcname !~ /^(.*)\@\.\@$/ ) { $completename = $funcname . $funcsuffix; }
	else {
	    my $loctype = "$1 ";
	    $funcsuffix =~ /^\_(.*)$/;
	    $completename = $loctype . $1;
	}

	$line = $completename . $function->{"parameters"} . $function->{"verbatim"} . "\n";

	my @names = split( / /, $completename );
	$completename = $names[@names - 1];

	print "\t\tCreating function $completename\n" if( $verbose );
	push( @Lines, $line );

	my $body = &splitNewlinesBuffer( $function->{"body"}, 0 );
	push( @Lines, @{$body} );

	push( @Lines, "\n" );
    }

    return( &parseValueAndType($parameter, $fullparname, $funcsuffix, $completename, 0, 0, @Lines) );
}

sub createCFunctionCode {
    (my $Class, my $function, my $parameter ) = @_;
    my @Lines = (); my $line = "";
    my $paramname = "\"" . $parameter->{"names"}->[0] . "\"";
    my $paramsuffix = &createSuffix( $parameter->{"names"}->[0], $parameter->{"functionname"} );
    my $funcname = $function->{"name"};
    my $completename = "";

    $funcname =~ s/\sexterns//;

    if( $style ) { $funcname =~ s/^(.*) ([\*\&]?)\s*([a-zA-Z\_]+)$/$1 $2$classprefix$3/; }
    else { $funcname =~ s/^(.*) ([\*\&]?)\s*([a-zA-Z\_]+)$/$1$2 $classprefix$3/; }

    if( $funcname =~ /^(.*)\@\.\@(.*)$/ ) {
	my $before = $1; my $after = $2;
	$paramsuffix =~ /^\_(.*)$/;
	$completename = $1 . $after;

	$line = $before . $completename . $function->{"parameters"} . $function->{"verbatim"} . "\n";
    }
    else { $line = $funcname . $paramsuffix . $function->{"parameters"} . $function->{"verbatim"} . "\n"; }

    my @names = split( / /, $completename );
    $completename = $names[@names - 1];

    print "\t\tCreating C wrapper function body $completename\n" if( $verbose );
    push( @Lines, $line );

    my $body = &splitNewlinesBuffer( $function->{"body"}, 0 );
    push( @Lines, @{$body} );
    push( @Lines, "\n" );

    return( &parseValueAndType($parameter, $paramname, $paramsuffix, $completename, $Class, $function->{"mapping"}, @Lines) );
}

sub createClassCodeBefore {
    (my $type, my $class) = @_;
    my @Lines = ();

    if( $type eq "namespace" ) {
	push( @Lines, "namespace " . $class->{"Name"} . "{\n" );
    }

    return( \@Lines );
}

sub createClassCodeAfter {
    (my $type, my $class) = @_;
    my @Lines = ();

    if( $type eq "namespace" ) {
	push( @Lines, "} // Namespace " . $class->{"Name"} . "\n" );
    }

    return( \@Lines );
}

sub createClassHeaderBefore {
    (my $type, my $class) = @_;
    my @Lines = ();

    if( $type eq "class" ) {
	push( @Lines, "class " . $class->{"Name"} . " {\n" );
	push( @Lines, "public:\n" );
    }
    elsif( $type eq "namespace" ) {
	push( @Lines, "namespace " . $class->{"Name"} . " {\n" );
    }
    push( @Lines, "\n" );


    return( \@Lines );
}

sub createClassHeaderAfter {
    (my $type, my $class) = @_;
    my @Lines = ();

    if( $type eq "class" ) { push( @Lines, "};\n" ); }
    elsif( $type eq "namespace" ) { push( @Lines, "} // Namespace " . $class->{"Name"} . "\n" ); }

    return( \@Lines );
}

sub createBeginCExternalLinkage {
    my @Lines = ( "\n#ifdef __cplusplus\n",
		  "extern \"C\" {\n",
		  "#endif\n\n" );

    return( \@Lines );
}

sub createEndCExternalLinkage {
    my @Lines = ( "\n#ifdef __cplusplus\n",
		  "} /* extern \"C\" */\n",
		  "#endif\n\n" );

    return( \@Lines );
}		  

sub createClassHeaderParameters {
    (my $type, my $Class, $tabs, $scope) = @_;
    my @Lines = (); my $paramdef = "";

    die "Cannot create empty class \"" . $Class->{"Name"} . "\".\n" if( @{$Class->{"Parameters"}} == 0 );

    $tabs += 2 if( ($tabs == 0) && ($Class->{"Name"} ne "0") );

    if( $type eq "class" ) { $paramdef = "static const std::string"; }
    elsif( $type eq "namespace" ) { $paramdef = "const std::string"; }

    foreach my $parameter ( @{$Class->{"Parameters"}} ) {
	if( ($parameter->{"class"}->{"Name"} eq $Class->{"Name"}) && 
	    (!defined($scope) || ($scope eq $parameter->{"class"}->{"Name"})) &&
	    $parameter->{"names"}->[1] && ($parameter->{"names"}->[1] !~ /^ref\[.+\]$/) ) {
	    my $headline = &tabs($tabs) . $paramdef . " " . $parameter->{"names"}->[1];

	    if( $type eq "class" ) { $headline .= ";\n"; }
	    elsif( $type eq "namespace" ) { $headline .= "( \"" . $parameter->{"names"}->[0] . "\" );\n"; }

	    push( @Lines, $headline );
	}
    }

    return( \@Lines );
}

sub selectFunctions {
    (my $parameter, my $allfunctions) = @_;
    my $type = $parameter->{"type"}; $type =~ s/\*/\\\*/g;
    my @functions = grep { grep(/^$type$/, @{$_->{"types"}}) } @{$allfunctions};

    @functions = grep { 
	my $el = $_;
	grep( /^$el->{"section"}$/, @{$parameter->{"sections"}} );
    } @functions;

    return( @functions );
}

sub createClassHeaderFunctions {
    (my $type, my $Class, $tabs, $scope) = @_;
    my @Lines = ();

    die "Cannot create empty class \"" . $Class->{"Name"} . "\".\n" if( @{$Class->{"Parameters"}} == 0 );
 
    $tabs += 2 if( ($tabs == 0) && ($Class->{"Name"} ne "0") );

    print "\tDealing with class \"" . $Class->{"Name"} . "\".\n" if( $verbose );

    foreach my $parameter ( @{$Class->{"Parameters"}} ) {
	next if( defined($scope) && ($parameter->{"class"}->{"Name"} ne $scope) );

	my @functions = &selectFunctions( $parameter, $Class->{"Functions"}, $scope );

	foreach my $func ( @functions ) {
	    my $lines = &createFunctionPrototype( $func, $parameter, $tabs );
	    push( @Lines, @{$lines} );
	}
    }

    return( \@Lines );
}

sub createClassCodeParameters {
    (my $type, my $Class, my $scope) = @_;
    my @Lines = ();
    my $classprefix = "";

    if( $type eq "class" ) {
	if( $Class->{"Name"} ne "0" ) { $classprefix = $Class->{"Name"} . "::"; }

	die "Cannot create empty class \"" . $Class->{"Name"} . "\".\n" if( @{$Class->{"Parameters"}} == 0 );

	foreach my $parameter ( @{$Class->{"Parameters"}} ) {
	    if( ($parameter->{"class"}->{"Name"} eq $Class->{"Name"}) && 
		(!defined($scope) || ($scope eq $parameter->{"class"}->{"Name"})) &&
		$parameter->{"names"}->[1] && ($parameter->{"names"}->[1] !~ /^ref\[.*\]$/) ) {
		my $codeline = "const std::string " . $classprefix . $parameter->{"names"}->[1] .
		    " = \"" . $parameter->{"names"}->[0] . "\";\n";

		push( @Lines, $codeline );
	    }
	}
    }

    return( \@Lines );
}

sub createClassCodeFunctions {
    (my $type, my $Class, my $scope) = @_;
    my @Lines = ();
    my $funcprefix = "";

    if( ($type eq "class") && ($Class->{"Name"} ne "0") ) { $funcprefix = $Class->{"Name"} . "::"; }

    die "Cannot create empty class \"" . $Class->{"Name"} . "\".\n" if( @{$Class->{"Parameters"}} == 0 );

    print "\tDealing with class \"" . $Class->{"Name"} . "\".\n" if( $verbose );

    foreach my $parameter ( @{$Class->{"Parameters"}} ) {
	next if( defined($scope) && ($parameter->{"class"}->{"Name"} ne $scope) );

	my @functions = &selectFunctions( $parameter, $Class->{"Functions"} );

	foreach my $func ( @functions ) {
	    my $lines = &createFunctionCode( $funcprefix, $func, $parameter );
	    push( @Lines, @{$lines} );
	}
    }

    return( \@Lines );
}

sub createClassCPrototypes {
    (my $Class, my $scope) = @_;
    my @Lines = (); my $buffer;

    die "Cannot create C wrappers for empty class \"" . $Class->{"Name"} . "\".\n" if( @{$Class->{"Parameters"}} == 0 );

    print "\tCreating C wrappers prototypes for class \"" . $Class->{"Name"} . "\".\n" if( $verbose );

    my $buffer = &createBeginCExternalLinkage();
    push( @Lines, @{$buffer} );

    foreach my $parameter ( @{$Class->{"Parameters"}} ) {
	next if( defined($scope) && ($parameter->{"class"}->{"Name"} ne $scope) );

	my @functions = &selectFunctions( $parameter, $Class->{"CFunctions"}, $scope );

	foreach my $func ( @functions ) {
	    my $lines = &createCFunctionPrototype( $Class, $func, $parameter );
	    push( @Lines, @{$lines} );
	}
    }

    my $buffer = &createEndCExternalLinkage();
    push( @Lines, @{$buffer} );

    return( \@Lines );
}

sub createClassCFunctions {
    (my $Class, my $scope) = @_;
    my @Lines = (); my $buffer;

    die "Cannot create C wrappers for empty class \"" . $Class->{"Name"} . "\".\n" if( @{$Class->{"Parameters"}} == 0 );

    print "\tCreating C wrappers function bodies for class \"" . $Class->{"Name"} . "\".\n" if( $verbose );

    my $buffer = &createBeginCExternalLinkage();
    push( @Lines, @{$buffer} );

    foreach my $parameter ( @{$Class->{"Parameters"}} ) {
	next if( defined($scope) && ($parameter->{"class"}->{"Name"} ne $scope) );

	my @functions = &selectFunctions( $parameter, $Class->{"CFunctions"}, $scope );

	foreach my $func ( @functions ) {
	    my $lines = &createCFunctionCode( $Class, $func, $parameter );
	    push( @Lines, @{$lines} );
	}
    }

    my $buffer = &createEndCExternalLinkage();
    push( @Lines, @{$buffer} );

    return( \@Lines );
}

sub createDisclaimer {
    my @Lines = ( "/**\n",
		  " *  WARNING !!!!\n",
		  " *  This is a generated file obtained with generator $version\n",
		  " *  Any modification you will made to this file will be lost\n",
		  " *  at the following rebuild of the source file. If you have to\n",
		  " *  modify anything inside there, consider to modify the definition\n",
		  " *  file $definitionFile or the template file " . $_[0] . "\n",
		  " *  that you should have found in the same directory.\n",
		  " **/\n" );

    return( @Lines );
}

sub createHeaderFile {
    (my $Def, my $file) = @_;
    my $header = my $line = "";
    my @Header = ( &createDisclaimer($file) );

    open( HEADTMPL, "<$file" ) || die "Cannot open template header \"$file\".\n";
    my @Template = <HEADTMPL>;
    close( HEADTMPL );

    if( $file =~ /^(.*)\.G$/ ) { $header = $1; }
    else { $header = $file . ".h"; }

    print "Creating header \"$header\".\n" if( $verbose );

    while( $line = shift(@Template) ) {
	if( $line =~ /^\s*\/\*\s*Tmpl:\s+include\s+(class|namespace)\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{$2} ) {
		unless( $3 ) {
		    my $before = &createClassHeaderBefore( $1, $Def->{$2} );
		    push( @Header, @{$before} );

		    my $class = &createClassHeaderParameters( $1, $Def->{$2}, 3 );
		    push( @Header, @{$class} );

		    push( @Header, "\n" );

		    my $class = &createClassHeaderFunctions( $1, $Def->{$2}, 3 );
		    push( @Header, @{$class} );

		    my $after = &createClassHeaderAfter( $1, $Def->{$2} );
		    push( @Header, @{$after} );
		}
		elsif( $3 eq "parameters" ) {
		    my $class = &createClassHeaderParameters( $1, $Def->{$2}, 3 );
		    push( @Header, @{$class} );
		}
		elsif( $3 eq "functions" ) {
		    my $class = &createClassHeaderFunctions( $1, $Def->{$2}, 3 );
		    push( @Header, @{$class} );
		}
		else { die "Unsupported part specification ($3) for $1 \"" . $Def->{$2}->{"Name"} . "\".\n"; }
	    }
	    else { die "Request for undefined $1 \"$2\". Aborting.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+partial\s+include\s+(class|namespace)+\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{$2} ) { 
		unless( $3 ) {
		    my $class = &createClassHeaderParameters( $1, $Def->{$2}, 0 );
		    push( @Header, @{$class} );

		    push( @Header, "\n" );

		    my $class = &createClassHeaderFunctions( $1, $Def->{$2}, 0 );
		    push( @Header, @{$class} );
		}
		elsif( $3 eq "parameters" ) {
		    my $class = &createClassHeaderParameters( $1, $Def->{$2}, 0 );
		    push( @Header, @{$class} );
		}
		elsif( $3 eq "functions" ) {
		    my $class = &createClassHeaderFunctions( $1, $Def->{$2}, 0 );
		    push( @Header, @{$class} );
		}
		else { die "Unsupported part specification ($3) for $1 \"" . $Def->{$2}->{"Name"} . "\".\n"; }
	    }
	    else { die "Request for undefined $1 \"$2\". Aborting.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+include\s+generic\.?([a-z]*)\s+for\s+(class|namespace)\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\s*\*\/$/ ) {
	    if( $Def->{"0"} ) {
		unless( $1 ) {
		    my $class = &createClassHeaderParameters( $2, $Def->{"0"}, 0, $3 );
		    push( @Header, @{$class} );

		    push( @Header, "\n" );

		    $class = &createClassHeaderFunctions( $2, $Def->{"0"}, 0, $3 );
		    push( @Header, @{$class} );
		}
		elsif( $1 eq "parameters" ) {
		    my $class = &createClassHeaderParameters( $2, $Def->{"0"}, 0, $3 );
		    push( @Header, @{$class} );
		}
		elsif( $1 eq "functions" ) {
		    my $class = &createClassHeaderFunctions( $2, $Def->{"0"}, 0, $3 );
		    push( @Header, @{$class} );
		}
		else { die "Unsupported part specification ($3) for $2 \"generic\".\n"; }
	    }
	    else { die "Undefined generic part requested.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+include\s+generic\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{"0"} ) {
		unless( $1 ) {
		    my $class = &createClassHeaderParameters( "class", $Def->{"0"}, 0 );
		    push( @Header, @{$class} );

		    push( @Header, "\n" );

		    $class = &createClassHeaderFunctions( "class", $Def->{"0"}, 0 );
		    push( @Header, @{$class} );
		}
		elsif( $1 eq "parameters" ) {
		    my $class = &createClassHeaderParameters( "class", $Def->{"0"}, 0 );
		    push( @Header, @{$class} );
		}
		elsif( $1 eq "functions" ) {
		    my $class = &createClassHeaderFunctions( "class", $Def->{"0"}, 0 );
		    push( @Header, @{$class} );
		}
		else { die "Unsupported part specification ($1) for class \"generic\".\n"; }
	    }
	    else { die "Undefined generic part requested.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+wrap\s+generic\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{"0"} ) {
		unless( $1 ) {
		    my $prototupes = &createClassCPrototypes( $Def->{"0"} );

		    push( @Header, @{$prototypes} );
		}
		else {
		    my $prototypes = &createClassCPrototypes( $Def->{"0"}, $1 );

		    push( @Header, @{$prototypes} );
		}
	    }
	    else { die "Undefined generic part requested.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+wrap\s+class\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{$1} ) {
		unless( $2 ) {
		    my $prototypes = &createClassCPrototypes( $Def->{$1} );

		    push( @Header, @{$prototypes} );
		}
		else {
		    my $prototypes = &createClassCPrototypes( $Def->{$1}, $2 );

		    push( @Header, @{$prototypes} );
		}
	    }
	    else { die "Request for undefined class \"$1\", aborting.\n"; }
	}
	else { push( @Header, $line ); }
    }

    &checkAndWriteFile( $header, @Header );
}

sub createCodeFile {
    (my $Def, my $file) = @_;
    my $code = my $line = "";
    my @Code = ( &createDisclaimer($file) );

    open( CODETMPL, "<$file" ) || die "Cannot open template code file \"$file\".\n";
    my @Template = <CODETMPL>;
    close( CODETMPL );

    if( $file =~ /^(.*)\.G$/ ) { $code = $1; }
    else { $code = $file . ".cpp"; }

    print "Creating code file \"$code\".\n" if( $verbose );

    while( $line = shift(@Template) ) {
	if( $line =~ /^\s*\/\*\s*Tmpl:\s+include\s+(class|namespace)\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{$2} ) {
		unless( $3 ) {
		    my $before = &createClassCodeBefore( $1, $Def->{$2} );
		    push( @Code, @{$before} );

		    my $class = &createClassCodeParameters( $1, $Def->{$2} );
		    push( @Code, @{$class} );

		    push( @Code, "\n" );

		    $class = &createClassCodeFunctions( $1, $Def->{$2} );
		    push( @Code, @{$class} );

		    my $after = &createClassCodeAfter( $1, $Def->{$2} );
		    push( @Code, @{$after} );
		}
		elsif( $3 eq "parameters" ) {
		    my $class = &createClassCodeParameters( $1, $Def->{$2} );
		    push( @Code, @{$class} );
		}
		elsif( $3 eq "functions" ) {
		    my $class = &createClassCodeFunctions( $1, $Def->{$2} );
		    push( @Code, @{$class} );
		}
		else { die "Unsupported part specification ($3) for $1 \"" . $Def->{$2}->{"Name"} . "\".\n"; }
	    }
	    else { die "Request for undefined $1 \"$2\". Aborting.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+include\s+generic\.?([a-z]*)\s+for\s+(class|namespace)\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\s*\*\/$/ ) {
	    if( $Def->{"0"} ) {
		unless( $1 ) {
		    my $class = &createClassCodeParameters( $2, $Def->{"0"}, $3 );
		    push( @Code, @{$class} );

		    push( @Code, "\n" );

		    $class = &createClassCodeFunctions( $2, $Def->{"0"}, $3 );
		    push( @Code, @{$class} );
		}
		elsif( $1 eq "parameters" ) {
		    my $class = &createClassCodeParameters( $2, $Def->{"0"}, $3 );
		    push( @Code, @{$class} );
		}
		elsif( $1 eq "functions" ) {
		    my $class = &createClassCodeFunctions( $2, $Def->{"0"}, $3 );
		    push( @Code, @{$class} );
		}
		else { die "Unsupported part specification ($1) for class \"generic\".\n"; }
	    }
	    else { die "Undefined generic part requested.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+include\s+generic\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{"0"} ) {
		unless( $1 ) {
		    my $class = &createClassCodeParameters( "class", $Def->{"0"} );
		    push( @Code, @{$class} );

		    push( @Code, "\n" );

		    $class = &createClassCodeFunctions( "class", $Def->{"0"} );
		    push( @Code, @{$class} );
		}
		elsif( $1 eq "parameters" ) {
		    my $class = &createClassCodeParameters( "class", $Def->{"0"} );
		    push( @Code, @{$class} );
		}
		elsif( $1 eq "functions" ) {
		    my $class = &createClassCodeFunctions( "class", $Def->{"0"} );
		    push( @Code, @{$class} );
		}
		else { die "Unsupported part specification ($1) for class \"generic\".\n"; }
	    }
	    else { die "Undefined generic part requested.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+wrap\s+generic\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{"0"} ) {
		unless( $1 ) {
		    my $functions = &createClassCFunctions( $Def->{"0"} );

		    push( @Code, @{$functions} );
		}
		else {
		    my $functions = &createClassCFunctions( $Def->{"0"}, $1 );

		    push( @Code, @{$functions} );
		}
	    }
	    else { die "Undefined generic part requested.\n"; }
	}
	elsif( $line =~ /^\s*\/\*\s*Tmpl:\s+wrap\s+class\s+([a-zA-Z\_][a-zA-Z0-9\_]*)\.?([a-z]*)\s*\*\/$/ ) {
	    if( $Def->{$1} ) {
		unless( $2 ) {
		    my $functions = &createClassCFunctions( $Def->{$1} );

		    push( @Code, @{$functions} );
		}
		else {
		    my $functions = &createClassCFunctions( $Def->{$1}, $2 );

		    push( @Code, @{$functions} );
		}
	    }
	    else { die "Request for undefined class \"$1\", aborting.\n"; }
	}
	else { push( @Code, $line ); }
    }

    &checkAndWriteFile( $code, @Code );
}

sub main() {
    my @args = @_;
    my $havedefs = 0;
    my $Definitions;

    &displayHelp( 1 ) if( @args == 0 );
    while( $arg = shift(@args) ) {
	if( $arg =~ /^\-(.*)/ ) {
	    if( ($1 eq "h") || ($1 eq "-help") ) { &displayHelp( 0 ); }
	    elsif( ($1 eq "c") || ($1 eq "-code") ) {
		&displayHelp( 1 ) unless( $havedefs );

		my $file = shift( @args );
		&displayHelp( 1 ) unless( $file );
		&createCodeFile( $Definitions, $file );
	    }
	    elsif( ($1 eq "H") || ($1 eq "-header") ) {
		&displayHelp( 1 ) unless( $havedefs );

		my $file = shift( @args );
		&displayHelp( 1 ) unless( $file );
		&createHeaderFile( $Definitions, $file );
	    }
	    elsif( ($1 eq "v") || ($1 eq "-verbose") ) { $verbose = 1; }
	    else { &displayHelp( 1 ); }
	}
	elsif( !$havedefs ) {
	    $Definitions = &loadDefinitions( $arg );
	    $havedefs = 1;
	}
	else { &displayHelp( 1 ); }
    }

    return( 0 );
}

&main( @ARGV );
