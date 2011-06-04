CC := g++
CFLAGS := -Wall -Wextra

S3_BIN := s3://davidhu-mr1/bin
S3_INPUT := s3://davidhu-aquaint
S3_OUTPUT := s3://davidhu-mr1/output
# We need a unique output directory name, otherwise AWS complains about
# existing output directory. Override this from the command line, as in
# $ make S3_OUTDIR=out_dir_name <target>
S3_OUTDIR := $(shell date +'%Y%m%d%H%M%S')
S3_LOG := s3://mapreduce-log-uri
EMR := /home/david/bin/elastic-mapreduce-ruby/elastic-mapreduce
ALL_BINS := mapper1 reducer1 reducer2 mapper3

# TODO: create more variables and reduce code duplication
# TODO: s3 output should be incremented each time or get from cmd line argument
# TODO: properly name files
# TODO: README file
# TODO: Instead of identity mapper, can we just get Hadoop to omit a map/reduce step?
# TODO: add dependent tools (elastic-mapreduce, credentials file, etc.) to repo

all: $(ALL_BINS)

run: job3

# Third job: Map that outputs the Jaccard similarity between all pairs of
# documents that share a shingle
job3: job2
	$(EMR) -j $(JOB_ID) \
		--stream \
		--mapper $(S3_BIN)/mapper3 \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j3 \
		--reducer cat \
		--jobconf 'mapred.task.timeout=60000'

# Second job: Just a reduce that sums all the docId pairs that share shingles,
# so we get <docId pair, # of common shingles>
job2: job1
	$(EMR) -j $(JOB_ID) \
		--stream \
		--mapper cat \
		--input $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j2 \
		--reducer $(S3_BIN)/reducer2 \
		--jobconf 'mapred.task.timeout=60000'

# First job: a map-reduce that takes in documents and outputs <docId pair, 1>
# for all pairs of documents that share a shingle, for each shingle they share
job1: bin-up
	$(EMR) -j $(JOB_ID) \
		--stream \
		--mapper $(S3_BIN)/mapper1 \
		--input $(S3_INPUT) \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-j1 \
		--reducer $(S3_BIN)/reducer1 \
		--jobconf 'mapred.task.timeout=60000'

start:
	$(EMR) --create --alive --enable-debugging --log-uri $(S3_LOG)

bin-up: $(ALL_BINS)
	s3cmd put $^ $(S3_BIN)/

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
