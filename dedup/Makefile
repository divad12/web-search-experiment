# This can be run on any *nix machine. Building binaries for Elastic MapReduce
# will take place on the master node, so ensure the master node is the same
# architecture as the slaves.

################################################################################
# HUMAN: Configure the parameters in this section for each run
################################################################################

# Name of this entire jobflow (when you start up instances until you stop them)
JOBFLOW_NAME := "4-m2.4xlarge-CatB"

# Number of instances, including master and slaves. If > 1, only slaves will
# run the map reduce binaries.
NUM_INSTANCES := 5

# For info on Amazon EC2 instance types, please see
# http://aws.amazon.com/elasticmapreduce/pricing/ and
# http://aws.amazon.com/ec2/instance-types/
# We've generally found that running on a low number of High-Memory Quadruple
# Extra Large (m2.4xlarge) machines to be the most cost-effective.
#
# NOTE: Binaries compiled on the master node must be able to be run on slave
#       nodes, so ensure that they are of the same platform type
MASTER_TYPE := m1.large
SLAVE_TYPE := m2.4xlarge

# The tag name of the XML element containing the text the de-duplication
# algorithm will analyze.
# For aquaint, use "BODY", and for clueweb use "cached" or "body"
CONTENT_TAG_NAME := cached

# The s3 input directory(-ies) where the gzipped input documents to
# de-duplicate are uploaded.
S3_INPUT := \
	s3://clueweb3/en0000 \
	s3://clueweb3/en0001 \
	s3://clueweb3/en0002 \
	s3://clueweb3/en0003 \
	s3://clueweb3/en0004 \
	s3://clueweb3/en0005 \
	s3://clueweb3/en0006 \
	s3://clueweb3/en0007 \
	s3://clueweb3/en0008 \
	s3://clueweb3/en0009 \
	s3://clueweb3/en0010 \
	s3://clueweb3/en0011 \
	s3://clueweb3/enwp00 \
	s3://clueweb3/enwp01 \
	s3://clueweb3/enwp02 \
	s3://clueweb3/enwp03 # "CatB"
#S3_INPUT := s3://clueweb/en0000  # 2 gb of the clueweb 09 collection
#S3_INPUT := s3://clueweb3/en0000  # 3 gb of the clueweb 09 collection, updated format (has <cached> tag)
#S3_INPUT := s3://smucker-mr/clueweb/00.trectext.gz  # 23 mb of clueweb for testing purposes
#S3_INPUT := s3://smucker-mr/aquaint/all/**/*  # 1 G - all of aquaint
#S3_INPUT := s3://smucker-mr/aquaint/all/xie/*  # 565 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/xie/*  # 565 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/apw/*  # 235 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/nyt/1998  # 155 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/xie/2000  # 41 mb
#S3_INPUT := s3://smucker-mr/aquaint/10mb
#S3_INPUT := s3://smucker-mr/aquaint/small


################################################################################
# Other variables

# Other S3 output directories that dedup will write to
S3_OUTPUT := s3://smucker-mr/output  # For map-reduce intermediate files and final output
S3_LOG := s3://smucker-mr/log  # For log files (output of stderr, Hadoop syslog, etc.)
S3_BIN := s3://smucker-mr/bin  # For MapReduce program binaries
S3_SRC := s3://smucker-mr/src  # For MapReduce program 
# We need a unique output directory name, otherwise AWS complains about
# existing output directory. Override this from the command line, as in
# $ make S3_OUTDIR=out_dir_name <target>
S3_OUTDIR := $(shell date +'%Y%m%d%H%M%S')

CC := g++
CFLAGS := -Wall -Wextra -O3
S3CMD := third_party/s3cmd/s3cmd -c third_party/s3cmd/.s3cfg
EMR := third_party/elastic-mapreduce-ruby/elastic-mapreduce --credentials third_party/elastic-mapreduce-ruby/credentials.json

LOCAL_INPUT := samples/*.gz
TESTS := tests/*.in
JOB_ID = $(shell cat job-id)
AWS_ACCESS_KEY_ID := $(shell sed '1q;d' aws-keys)
AWS_SECRET_ACCESS_KEY := $(shell sed '2q;d' aws-keys)

ALL_BINS := mapper1 reducer1 reducer2 mapper-cluster reducer-cluster

# TODO: Add more tests
# TODO: Hadoop configuration options (See Hani's thesis and "Hadoop: The Definitive Guide")
# TODO: figure out why sequencefileformat doesn't work (output not compressed. if compression type set to BLOCK, second reducer craps out)

all: $(ALL_BINS)


################################################################################
# Local run targets

# Simple functional testing, useful for detecting regressions. Simply does a
# diff of final MapReduce output against pre-computed output in text files
test: $(ALL_BINS)
	@for i in $(TESTS); do \
		test=$${i%.*}; \
		tmp="$$test".tmp; \
		cat $$i | $(MAKE) -s CONTENT_TAG_NAME=body local | cut -f2- > $$tmp; \
		diff -u "$$test".out $$tmp \
			&& echo PASSED: `basename $$test` \
			&& rm "$$tmp" \
			|| echo ***FAILED: `basename $$test`; \
	done

local: $(ALL_BINS)
	./mapper1 $(CONTENT_TAG_NAME) \
		| sort \
		| ./reducer1 \
		| sort \
		| ./reducer2 \
		| ./mapper-cluster \
		| sort \
		| ./reducer-cluster

local-debug: $(ALL_BINS)
	./mapper1 $(CONTENT_TAG_NAME) | sort > out/mapper1.out
	./reducer1 < out/mapper1.out | sort > out/reducer1.out
	./reducer2 < out/reducer1.out > out/reducer2.out
	./mapper-cluster < out/reducer2.out | sort > out/mapper-cluster.out
	./reducer-cluster < out/mapper-cluster.out > out/reducer-cluster.out


################################################################################
# Run status query targets

log:
	$(EMR) -j $(JOB_ID) --logs

ssh:
	$(EMR) -j $(JOB_ID) --ssh

# Parse and output runnning job data in a readable form
report:
	$(MAKE) -s describe > reports/$(JOB_ID).json
	./parse-data.py < reports/$(JOB_ID).json > reports/$(JOB_ID)-report
	$(EMR) -j $(JOB_ID) --ssh 'cat /mnt/var/log/hadoop/steps/**/syslog' > reports/$(JOB_ID).log

# Describe the running jobs in detail. Returns JSON
describe:
	$(EMR) -j $(JOB_ID) --describe --active

# List the active jobs in the current jobflow
list:
	$(EMR) -j $(JOB_ID) --list --active


################################################################################
# Amazon Elastic MapReduce run targets

stop:
	$(EMR) -j $(JOB_ID) --terminate
	rm job-id

run: job3

# Third job: Output clusters (of size > 1) of docs that are near-duplicates.
job3: job2
	FINAL_OUTPUT := $(S3_OUTPUT)/$(S3_OUTDIR)-j3
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "<docId pair, # common shingles> --> clusters of similar docs" \
		--mapper $(S3_BIN)/mapper-cluster \
		--reducer $(S3_BIN)/reducer-cluster \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--output $(FINAL_OUTPUT) \
		--jobconf mapred.output.compress=true
	@echo "Final output will be in $(FINAL_OUTPUT)"

# Second job: Just a reduce that sums all the docId pairs that share shingles,
# so we get <docId pair, # of common shingles>
job2: job1
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "<docId pair, 1> --> <docId pair, # common shingles>" \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--mapper cat \
		--reducer $(S3_BIN)/reducer2 \
		--args "-D,mapred.output.compress=true" \
		--args "-D,mapred.compress.map.output=true" \
		--args "-D,mapred.task.timeout=3000000" \
		#--args "-D,io.sort.mb=1500" \
		#--args "-D,io.sort.factor=100" \
		#--args "-D,mapred.reduce.parallel.copies=50"

# First job: a map-reduce that takes in documents and outputs <docId pair, 1>
# for all pairs of documents that share a shingle, for each shingle they share
# We use a hack (pipe to sort then reducer2) as a local combiner, because
# hadoop 0.2 still requires java class for combiner
job1: build-on-master
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "docs --> <docId pair, 1>" \
		--mapper $(S3_BIN)/mapper1 $(CONTENT_TAG_NAME) \
		$(foreach dir,$(S3_INPUT),--args "-input,$(dir)") \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--reducer "$(S3_BIN)/reducer1 | sort | $(S3_BIN)/reducer2" \
		--args "-D,mapred.output.compress=true" \
		--args "-D,mapred.compress.map.output=true" \
		--args "-D,mapred.task.timeout=1800000" \
		#--args "-D,io.sort.mb=1500" \
		#--args "-D,io.sort.factor=100" \
		#--args "-D,mapred.reduce.parallel.copies=50"

# Create a jobflow, configured with our supplied S3 keys.
# We don't put configuration keys in core-site.xml file on S3, because hadoop
# can't access S3 yet without these credentials.
start:
	$(EMR) --create \
		--alive \
		--name $(JOBFLOW_NAME) \
		--log-uri $(S3_LOG) \
		--num-instances $(NUM_INSTANCES) \
		--master-instance-type $(MASTER_TYPE) \
		--slave-instance-type $(SLAVE_TYPE) \
		--plain-output \
		--bootstrap-action "s3://elasticmapreduce/bootstrap-actions/configure-hadoop" \
		--args "--core-key-value,fs.s3.awsAccessKeyId=$(AWS_ACCESS_KEY_ID)" \
		--args "--core-key-value,fs.s3.awsSecretAccessKey=$(AWS_SECRET_ACCESS_KEY)" \
		--args "--core-key-value,fs.s3n.awsAccessKeyId=$(AWS_ACCESS_KEY_ID)" \
		--args "--core-key-value,fs.s3n.awsSecretAccessKey=$(AWS_SECRET_ACCESS_KEY)" \
		--args "--core-key-value,fs.s3bfs.awsAccessKeyId=$(AWS_ACCESS_KEY_ID)" \
		--args "--core-key-value,fs.s3bfs.awsSecretAccessKey=$(AWS_SECRET_ACCESS_KEY)" \
		| tee /dev/tty > job-id

# This is to be run on the Hadoop master instance
bin-up: $(ALL_BINS)
	# Hadoop is so stupid to require 3 steps for this...
	hadoop fs -rmr $(S3_BIN)
	hadoop fs -mkdir $(S3_BIN)
	hadoop fs -put $^ $(S3_BIN)

# Copy files (via s3) to the Hadoop master instance, build there, and then upload to s3
build-on-master: $(ALL_BINS)
	$(S3CMD) put Makefile *.cpp *.h $(S3_SRC)/
	$(S3CMD) -r put third_party/rabinhash-64/ $(S3_SRC)/third_party/rabinhash-64/
	$(EMR) -j $(JOB_ID) --ssh '"rm -rf src; hadoop fs -get $(S3_SRC) src; cd src; make bin-up"'
	touch $@


################################################################################
# Hadoop streaming binaries

mapper-reducer: mapper-reducer.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper-cluster: mapper-cluster.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer2: reducer2.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer1: reducer1.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper1: mapper1.cpp constants.h utils.o rabin-hash-64.o 
	$(CC) $(CFLAGS) -o $@ $^

utils: utils.cpp utils.h
	$(CC) $(CFLAGS) -c -o $@ $^

rabin-hash-64: rabin-hash-64.cpp rabin-hash-64.h
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rfv *.tmp *.o $(ALL_BINS)
