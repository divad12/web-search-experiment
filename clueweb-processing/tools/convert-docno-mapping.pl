
#
# this simple script reads in Jimmy Lin's docno.mapping file
# and converts it to an easy format to allow a simple hash to 
# offset 
#
# June 23, 2011
# Mark D. Smucker
# 

while ( $line = <> )
{
    chomp($line) ;
    @fields = split( /,/,$line) ;
    $numFields = @fields ;
    if ( $numFields != 4 )
    {
	die( "should be 4 columns" ) ;
    }

    $dir = $fields[0] ;
    $file = $fields[1] ;
    $numDocsInFile = $fields[2] ;
    $offset = $fields[3] ;

    printf( "%s-%02s\t%d\n",$dir,$file,$offset ) ;
}
