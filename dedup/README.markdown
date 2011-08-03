A C++ Hadoop streaming MapReduce for Amazon Web Services Elastic MapReduce to find clusters of near-duplicate documents. Useful for, eg., processing web crawls, for which usually between 10-30% of pages are near-duplicates. Currently, the algorithm is based on [Broder's original implementation for AltaVista](http://www.hpl.hp.com/techreports/Compaq-DEC/SRC-TN-1997-015.pdf).

# Input format
- Input is required to be in gzipped files on S3 (use `s3cmd` to upload files).
- Each file can contain multiple documents - all documents from all files will be analyzed to find near-duplicates.
- Each document must be in an XML format:

        <DOC>
          <DOCNO>uniqueDocumentIdentifier</DOCNO>
          <body>The text of the document to analyze...</body>
        </DOC>
  Tag names are case-sensitive! You must tell dedup the name of the main content node containing the document text (in the example above it is `<body>`) by setting the `CONTENT_TAG_NAME` variable in the Makefile (see Step 1 of section **Setup**).
- Each document must have a unique document ID string of no more than 1000 characters (the shorter, the faster). This string cannot contain any of:
    - whitespace (including newline, tab character, spaces)
    - `|` (pipe character)
    - `=` (equal sign)

See tests/\*.in for examples of valid input.

# Dependencies
- Ruby interpreter
- Amazon Web Services account with MapReduce and S3 (and SimpleDB for Hadoop debugging)

*s3cmd* (for interactions with S3) and *elastic-mapreduce* (ruby command line tool for interacting with Amazon EMR) are included in this source.

# Setup
1. Insert your AWS credentials in third\_party/elastic-mapreduce-ruby/credentials.json. See credentials.json.example for a sample.
2. Insert AWS credentials in third\_party/s3cmd/.s3cfg. See s3cfg.example for a sample. Alternatively, run `s3cmd --configure`.
3. Insert AWS credentials in file aws-keys (see aws-keys.example).
4. [To prevent ssh from prompting for host key checking](http://stackoverflow.com/questions/1655815), add the following to your ~/.ssh/config:

        Host *amazonaws.com
                StrictHostKeyChecking no
                UserKnownHostsFile /dev/null
  This will allow the commands below to be automatic and require minimal human input.

# Running
1. Change variables at the top of the Makefile (see in-file comments for details) to customize each run.
2. `make start` to start a new jobflow. This will stay alive until you terminate it (step 4).
3. `make run` to compile and run a job on AWS. You can run multiple jobs per jobflow. Repeat this step as desired.
4. `make stop` will terminate the jobflow when you are done running jobs.
5. Download the final output from S3 (step 3 will tell you where the output ends up). Then, run `zcat *.gz | sort -nr -k 1 | ./gen-html-clusters.py` to generate basic static HTML pages to display the clusters (currently links are to Mark's server that serves up cached clueweb09 docs). TODO: automate this step.

# Querying status
These commands help you monitor and get statistics regarding the currently running MapReduce jobs on Amazon EMR. These commands will only work after invocation of `make start` and before `make stop`.

- `make list` to view a summary of current jobflow: lists all steps their status.
- `make log` to view logs of the most recent step.
- `make ssh` to ssh into the master node. `/mnt/var/log/hadoop` is a good place to look once in.
- `make describe` will output detailed statistics of the active jobflow in JSON form.
- `make report` to download a summary of run stats into a human-readable form. Reports are put in reports/.

To ssh into a slave node (eg. to monitor instance system resource usage with `top`):

`ssh -i <your Amazon ssh key> hadoop@<slave instance public DNS>`. You can find DNS addresses on the AWS EC2 console.

# Local testing and debugging
- `zcat your_input_filename_here.gz | make -s local` will run the MapReduce locally with the given (gzipped) input file.
- `zcat input_file.gz | make local-debug` will run locally and save all intermediate files in out/.
- `make test` to run against test cases. This runs a simple functional test suite which simply does a diff of the final output of all the map-reduce steps against a previous run. Test cases are stored in text files in tests/. Useful for regression testing changes and new optimizations.
- To add a test case, just create the file pair &lt;name&gt;.{in,out}. &lt;name&gt;.in should contain the input in the XML doc format, and &lt;name&gt;.out should contain the expected mapreduced output.

# Advanced tweaking
You can customize algorithm parameters by modifying the file constants.h. Binaries will be automatically recompiled on the master node when you invoke `make run`. Further, you may wish to experiment with additional Hadoop configuration settings by modifying the **Amazon Elastic MapReduce** section of the Makefile.
