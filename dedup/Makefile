CC := g++
CFLAGS := -Wall -Wextra

ALL_BINS := mapper1 reducer1 reducer2 mapper3

S3CMD := third_party/s3cmd/s3cmd -c third_party/s3cmd/.s3cfg
EMR := third_party/elastic-mapreduce-ruby/elastic-mapreduce

S3_BIN := s3://smucker-mr/bin
S3_INPUT := s3://smucker-mr/aquaint/small
S3_OUTPUT := s3://smucker-mr/output
S3_LOG := s3://smucker-mr/log
S3_CONF := s3://smucker-mr/conf
# We need a unique output directory name, otherwise AWS complains about
# existing output directory. Override this from the command line, as in
# $ make S3_OUTDIR=out_dir_name <target>
S3_OUTDIR := $(shell date +'%Y%m%d%H%M%S')

LOCAL_INPUT := samples/*.gz
TESTS := tests/*.in
JOB_ID = $(shell cat job-id)
AWS_ACCESS_KEY_ID := $(shell sed '1q;d' aws-keys)
AWS_SECRET_ACCESS_KEY := $(shell sed '2q;d' aws-keys)

# TODO: Instead of identity mapper, can we just get Hadoop to omit a map/reduce step?
# TODO: terminate target

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
		| ./mapper3 \
		| sort -n -k 2

run: job3

# Third job: Map that outputs the Jaccard similarity between all pairs of
# documents that share a shingle
job3: job2
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "<docId pair, # common shingles> --> <docId pair, jaccard similarity>" \
		--mapper $(S3_BIN)/mapper3 \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j3 \
		--reducer cat

# Second job: Just a reduce that sums all the docId pairs that share shingles,
# so we get <docId pair, # of common shingles>
job2: job1
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "<docId pair, 1> --> <docId pair, # common shingles>" \
		--mapper cat \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--reducer $(S3_BIN)/reducer2 \

# First job: a map-reduce that takes in documents and outputs <docId pair, 1>
# for all pairs of documents that share a shingle, for each shingle they share
job1: bin-up
	$(EMR) -j $(JOB_ID) \
		--stream \
		--step-name "docs --> <docId pair, 1>" \
		--mapper $(S3_BIN)/mapper1 \
		--input $(S3_INPUT) \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--reducer $(S3_BIN)/reducer1

# Create a jobflow, configured with our supplied S3 keys.
# We don't put configuration keys in core-site.xml file on S3, because hadoop
# can't access S3 yet without these credentials.
start: config-up
	$(EMR) --create \
		--alive \
		--log-uri $(S3_LOG) \
		--bootstrap-action "s3://elasticmapreduce/bootstrap-actions/configure-hadoop" \
		--args "--core-key-value,fs.s3.awsAccessKeyId=$(AWS_ACCESS_KEY_ID)" \
		--args "--core-key-value,fs.s3.awsSecretAccessKey=$(AWS_SECRET_ACCESS_KEY)" \
		--args "--core-key-value,fs.s3n.awsAccessKeyId=$(AWS_ACCESS_KEY_ID)" \
		--args "--core-key-value,fs.s3n.awsSecretAccessKey=$(AWS_SECRET_ACCESS_KEY)" \
		--args "--core-key-value,fs.s3bfs.awsAccessKeyId=$(AWS_ACCESS_KEY_ID)" \
		--args "--core-key-value,fs.s3bfs.awsSecretAccessKey=$(AWS_SECRET_ACCESS_KEY)" \
		| tee /dev/tty \
		| cut -d' ' -f 4 > job-id

config-up: core-site.xml
	$(S3CMD) put $^ $(S3_CONF)/
	touch $@

bin-up: $(ALL_BINS)
	$(S3CMD) put $^ $(S3_BIN)/
	touch $@

mapper3: mapper3.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer2: reducer2.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer1: reducer1.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper1: mapper1.cpp rabinhash64.o
	$(CC) $(CFLAGS) -o $@ $^

rabinhash64.o: third_party/rabinhash-64/rabinhash64.cpp
	$(CC) $(CFLAGS) -c $^

clean:
	rm -r *.test.out *.o $(ALL_BINS)
