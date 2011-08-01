TODO: Update everything!!
- special delimiters used (cannot appear in DocIDs): '=', '|', '\t', '\n'
- DocIDs don't exceed 1000 chars (also make this into a constant)
- separate out rabin-hash-64.h

A MapReduce for Amazon Web Services Elastic MapReduce to find near-duplicate files. Input files must be in a given format (to specify later, see tests/ for now). This is still a work-in-progress.

# Dependencies
- Ruby interpreter
- Amazon Web Services account with MapReduce and S3 (and SimpleDB for Hadoop debugging)

# Setup
1. Insert your AWS credentials in third\_party/elastic-mapreduce-ruby/credentials.json. See credentials.json.example for a sample.
2. Insert AWS credentials in third\_party/s3cmd/.s3cfg. See s3cfg.example for a sample. Alternatively, run `s3cmd --configure`.
3. Insert AWS credentials in file aws-keys (see aws-keys.example).
4. Change the Makefile variables to your S3 directories where the MapReduce will take input and output to.

# Running
1. `make start` to start a new jobflow. This will stay alive until you terminate it (see step 3).
2. `make run` to run a job on AWS. You can run multiple jobs per jobflow.
3. `third_party/elastic-mapreduce-ruby/elastic-mapreduce -j <Job ID> --terminate` will terminate the jobflow when you are done running jobs.

(TODO: I'll add a makefile target for terminating and not require you to enter job id for each command.)

# Local Testing
`zcat your_input_filename_here.gz | make -s local` will run the MapReduce locally with the given (gzipped) input file.

`make test` to run against test cases.

To add a test case, just create the file pair &lt;name&gt;.{in,out}. &lt;name&gt;.in should contain the input in the XML doc format, and &lt;name&gt;.out should contain the expected mapreduced output.
