# This can be run on any *nix machine. Building binaries for Elastic MapReduce
# will take place on the master node, so ensure the master node is the same
# architecture as the slaves.

# Configure the parameters below
JOBFLOW_NAME := "2-cc1.4xlarge-clueweb3"
NUM_INSTANCES := 2
MASTER_TYPE := m1.large
# TODO: Test on High Performance Cluster Compute (cc1.4xlarge), high cpu xlarge
# (c1.xlarge), high memory xlarge (m2.4xlarge), etc. Make some graphs.
#SLAVE_TYPE := m1.xlarge
#SLAVE_TYPE := m2.4xlarge
SLAVE_TYPE := cc1.4xlarge

#S3_INPUT := s3://clueweb/en0000  # 2 gb of the clueweb 09 collection
S3_INPUT := s3://clueweb3/en0000  # 3 gb of the clueweb 09 collection, updated format (has <cached> tag)
#S3_INPUT := s3://smucker-mr/clueweb/00.trectext.gz  # 23 mb of clueweb for testing purposes
#S3_INPUT := s3://smucker-mr/aquaint/all/**/*  # 1 G - all of aquaint
#S3_INPUT := s3://smucker-mr/aquaint/all/xie/*  # 565 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/xie/*  # 565 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/apw/*  # 235 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/nyt/1998  # 155 mb
#S3_INPUT := s3://smucker-mr/aquaint/all/xie/2000  # 41 mb
#S3_INPUT := s3://smucker-mr/aquaint/10mb
#S3_INPUT := s3://smucker-mr/aquaint/small

S3_OUTPUT := s3://smucker-mr/output
S3_LOG := s3://smucker-mr/log
S3_BIN := s3://smucker-mr/bin
S3_SRC := s3://smucker-mr/src


CC := g++
CFLAGS := -Wall -Wextra -O3

ALL_BINS := mapper1 reducer1 reducer2 mapper3 mapper-cluster reducer-cluster

S3CMD := third_party/s3cmd/s3cmd -c third_party/s3cmd/.s3cfg
EMR := third_party/elastic-mapreduce-ruby/elastic-mapreduce --credentials third_party/elastic-mapreduce-ruby/credentials.json

# We need a unique output directory name, otherwise AWS complains about
# existing output directory. Override this from the command line, as in
# $ make S3_OUTDIR=out_dir_name <target>
S3_OUTDIR := $(shell date +'%Y%m%d%H%M%S')

LOCAL_INPUT := samples/*.gz
TESTS := tests/*.in
JOB_ID = $(shell cat job-id)
AWS_ACCESS_KEY_ID := $(shell sed '1q;d' aws-keys)
AWS_SECRET_ACCESS_KEY := $(shell sed '2q;d' aws-keys)

# TODO: Instead of identity mapper, can we just get Hadoop to omit a map or a reduce step?
# TODO: Split out common code to utils file
# TODO: Add more tests
# TODO: better names for mapper/reducer binaries
# TODO: Hadoop configuration options (See Hani's thesis)
# TODO: figure out why sequencefileformat doesn't work (output not compressed. if compression type set to BLOCK, second reducer craps out)

all: $(ALL_BINS)

# Simple functional testing, useful for detecting regressions
test: $(ALL_BINS)
	@for i in $(TESTS); do \
		test=$${i%.*}; \
		tmp="$$test".tmp; \
		cat $$i | $(MAKE) -s local > $$tmp; \
		diff -u "$$test".out $$tmp \
			&& echo PASSED: `basename $$test` \
			&& rm "$$tmp" \
			|| echo ***FAILED: `basename $$test`; \
	done

local: $(ALL_BINS)
	./mapper1 \
		| sort \
		| ./reducer1 \
		| sort \
		| ./reducer2 \
		| ./mapper-cluster \
		| sort \
		| ./reducer-cluster

local-debug: $(ALL_BINS)
	./mapper1 | sort > out/mapper1.out
	./reducer1 < out/mapper1.out | sort > out/reducer1.out
	./reducer2 < out/reducer1.out > out/reducer2.out
	./mapper-cluster < out/reducer2.out | sort > out/mapper-cluster.out
	./reducer-cluster < out/mapper-cluster.out > out/reducer-cluster.out

stop:
	$(EMR) -j $(JOB_ID) --terminate
	rm job-id

logs:
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

run: job3

# Third job: Output clusters (of size > 1) of docs that are near-duplicates.
job3: job2
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "<docId pair, # common shingles> --> clusters of similar docs" \
		--mapper $(S3_BIN)/mapper-cluster \
		--reducer $(S3_BIN)/reducer-cluster \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j3 \
		--jobconf mapred.output.compress=true

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
		--mapper $(S3_BIN)/mapper1 \
		--input $(S3_INPUT) \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--reducer "$(S3_BIN)/reducer1 | sort | $(S3_BIN)/reducer2" \
		--args "-D,mapred.output.compress=true" \
		--args "-D,mapred.compress.map.output=true" \
		--args "-D,mapred.task.timeout=1800000" \
		#--args "-D,io.sort.mb=1500" \
		#--args "-D,io.sort.factor=100" \
		#--args "-D,mapred.reduce.parallel.copies=50" \

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

mapper-reducer: mapper-reducer.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper-cluster: mapper-cluster.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper3: mapper3.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer2: reducer2.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer1: reducer1.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper1: mapper1.cpp
	$(CC) $(CFLAGS) -o $@ $^

#rabinhash64.o: third_party/rabinhash-64/rabinhash64.cpp
	#$(CC) $(CFLAGS) -c $^

# TODO: CLean up rabin-hash-64.h and put it in its own folder, etc. LICENSE.

clean:
	rm -rfv *.tmp *.o $(ALL_BINS)
