# This can be run on any *nix machine. Building binaries for Elastic MapReduce
# will take place on the master node, so ensure the master node is the same
# architecture as the slaves.

################################################################################
# HUMAN: Configure the parameters in this section for each run
################################################################################

# Name of this entire jobflow (when you start up instances until you stop them)
JOBFLOW_NAME := "sct-fix2-experiment-2"

# Number of instances, including master and slaves. If > 1, only slaves will
# run the map reduce binaries.
NUM_INSTANCES := 3

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

# If there's a common prefix of all DocIDs, specify it here to give a small
# speed optimization. Otherwise leave blank.
COMMON_DOCID_PREFIX := clueweb09-en

# The s3 input directory(-ies) where the gzipped input documents to
# de-duplicate are uploaded.
S3_INPUT := s3://clueweb3/en0000
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

# For map-reduce intermediate files and final output
S3_OUTPUT := s3://smucker-mr/output
# For log files (output of stderr, Hadoop syslog, etc.)
S3_LOG := s3://smucker-mr/log
# For MapReduce program binaries
S3_BIN := s3://smucker-mr/bin
# For MapReduce program
S3_SRC := s3://smucker-mr/src
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
AWS_ACCESS_KEY_ID := $(shell head -n 1 aws-keys)
AWS_SECRET_ACCESS_KEY := $(shell tail -n 1 aws-keys)

ALL_BINS := mapper-shingle reducer-all-pairs reducer-sum mapper-cluster reducer-cluster reducer-doc-size reducer-common-shingle

# TODO: Add more tests
# TODO: Hadoop configuration options (See Hani's thesis and "Hadoop: The Definitive Guide")

all: $(ALL_BINS)


################################################################################
# Local run targets

# Simple functional testing, useful for detecting regressions. Simply does a
# diff of final MapReduce output against pre-computed output in text files
test: $(ALL_BINS)
	@for i in $(TESTS); do \
		test=$${i%.*}; \
		tmp="$$test".tmp; \
		cat $$i | $(MAKE) -s CONTENT_TAG_NAME=body COMMON_DOCID_PREFIX="" local | cut -f2- > $$tmp; \
		diff -u "$$test".out $$tmp \
			&& echo PASSED: `basename $$test` \
			&& rm "$$tmp" \
			|| echo ***FAILED: `basename $$test`; \
	done

local: $(ALL_BINS)
	./mapper-shingle $(CONTENT_TAG_NAME) $(COMMON_DOCID_PREFIX) \
		| sort \
		| ./reducer-common-shingle \
		| sort \
		| ./reducer-doc-size \
		| sort \
		| ./reducer-all-pairs \
		| sort \
		| ./reducer-sum \
		| ./mapper-cluster \
		| sort \
		| ./reducer-cluster $(COMMON_DOCID_PREFIX)

local-debug: $(ALL_BINS)
	./mapper-shingle $(CONTENT_TAG_NAME) $(COMMON_DOCID_PREFIX) | sort > out/mapper-shingle.out
	./reducer-common-shingle < out/mapper-shingle.out | sort > out/reducer-common-shingle.out
	./reducer-doc-size < out/reducer-common-shingle.out | sort > out/reducer-doc-size.out
	./reducer-all-pairs < out/reducer-doc-size.out | sort > out/reducer-all-pairs.out
	./reducer-sum < out/reducer-all-pairs.out > out/reducer-sum.out
	./mapper-cluster < out/reducer-sum.out | sort > out/mapper-cluster.out
	./reducer-cluster $(COMMON_DOCID_PREFIX) < out/mapper-cluster.out > out/reducer-cluster.out


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

run: job5

# TODO: specify that we only want one reduce task
job5: job4
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "clustering" \
		--mapper $(S3_BIN)/mapper-cluster \
		--reducer "$(S3_BIN)/reducer-cluster $(COMMON_DOCID_PREFIX)" \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)/j4 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)/j5 \
		--args "-D,mapred.task.timeout=3000000" \
		--args "-D,mapred.output.compress=true"
	@echo "\nJob started! Final output will be in $(S3_OUTPUT)/$(S3_OUTDIR)/j5"

job4: job3
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "reducer-sum" \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)/j3 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)/j4 \
		--mapper cat \
		--reducer $(S3_BIN)/reducer-sum \
		--args "-D,mapred.output.compress=true" \
		--args "-D,mapred.compress.map.output=true" \
		--args "-D,mapred.task.timeout=3000000" \
		#--args "-D,io.sort.mb=1500" \
		#--args "-D,io.sort.factor=100" \
		#--args "-D,mapred.reduce.parallel.copies=50"

job3: job2
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "reducer-all-pairs" \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)/j2 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)/j3 \
		--mapper cat \
		--reducer $(S3_BIN)/reducer-all-pairs \
		--args "-D,mapred.output.compress=true" \
		--args "-D,mapred.compress.map.output=true" \
		--args "-D,mapred.task.timeout=3000000" \
		#--args "-D,io.sort.mb=1500" \
		#--args "-D,io.sort.factor=100" \
		#--args "-D,mapred.reduce.parallel.copies=50"

job2: job1
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "reducer-doc-size" \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)/j1 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)/j2 \
		--mapper cat \
		--reducer $(S3_BIN)/reducer-doc-size \
		--args "-D,mapred.output.compress=true" \
		--args "-D,mapred.compress.map.output=true" \
		--args "-D,mapred.task.timeout=3000000" \
		#--args "-D,io.sort.mb=1500" \
		#--args "-D,io.sort.factor=100" \
		#--args "-D,mapred.reduce.parallel.copies=50"

job1: build-on-master
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "mapper-shingle and reducer-common-shingle" \
		--mapper "$(S3_BIN)/mapper-shingle $(CONTENT_TAG_NAME) $(COMMON_DOCID_PREFIX)" \
		$(foreach dir,$(S3_INPUT),--args "-input,$(dir)") \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)/j1 \
		--reducer $(S3_BIN)/reducer-common-shingle \
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
# Hadoop is so stupid to require 3 steps for this...
bin-up: $(ALL_BINS)
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
mapper-cluster: mapper-cluster.cpp
reducer-doc-size: reducer-doc-size.cpp constants.h utils.o
reducer-common-shingle: reducer-common-shingle.cpp constants.h
reducer-sum: reducer-sum.cpp
reducer-all-pairs: reducer-all-pairs.cpp
mapper-shingle: mapper-shingle.cpp constants.h utils.o rabin-hash-64.o
utils: utils.cpp utils.h
rabin-hash-64: rabin-hash-64.cpp rabin-hash-64.h

clean:
	rm -rfv *.tmp *.o $(ALL_BINS)
