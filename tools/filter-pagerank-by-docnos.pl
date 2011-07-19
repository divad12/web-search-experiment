
use Getopt::Std;

# -t ARG sets $opt_t
#
getopt("d");

if ( ! $opt_d )
{
    print "cat input-col1-has-docno | perl -w filter-by-docnos.pl -d ok-docnos-file > filtered-input\n" ; 
    exit(0) ;
}

%docnosSet = () ;

# read in docnos
open( DOCNOS, "<$opt_d" ) || die( "unable to open $opt_d" ) ;
while ( $line = <DOCNOS> )
{
    chomp($line) ;
    $docnosSet{$line} = 0 ;
}
close(DOCNOS) ;

$minLogPrior = 0 ;

while ( $line = <> )
{
    chomp($line) ;
    @fields = split( /\s+/, $line ) ;
    $numFields = @fields ;
    if ( $numFields != 2 )
    {
	die( "should be 2 columns in pagerank input\n" ) ;
    }
    $docno = $fields[0] ;
    $prior = $fields[1] ;
    if ( $minLogPrior > $prior )
    {
	$minLogPrior = $prior ;
    }

    if ( exists( $docnosSet{$docno} ) )
    {
	$docnosSet{$docno} += 1 ;
	if ( $docnosSet{$docno} == 1 )
	{
	    print "$line\n" ;
	}
	else
	{
	    print STDERR "duplicated\t$docno\n" ;
	}
    }
}

foreach $docno ( keys %docnosSet )
{
    $val = $docnosSet{$docno} ;
    if ( $val == 0 )
    {
	print STDERR "missing\t$docno\tusing-min-prior=$minLogPrior\n" ;
	print "$docno $minLogPrior\n" ;
    }
}


	 
