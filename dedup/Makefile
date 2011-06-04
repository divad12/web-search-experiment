CC = g++
CFLAGS = -Wall -Wextra

S3_BIN = s3://davidhu-mr1/bin
S3_INPUT = s3://davidhu-aquaint
S3_OUTPUT = s3://davidhu-mr1/output
# We need a unique output directory name, otherwise AWS complains about
# existing output directory. Override this from the command line, as in
# $ make S3_OUTDIR=out_dir_name <target>
# FIXME: Hack hack hack
S3_OUTDIR = $(shell date +'%Y%m%d%H%M')
S3_LOG = s3://mapreduce-log-uri
EMR = /home/david/bin/elastic-mapreduce-ruby/elastic-mapreduce

# TODO: create more variables and reduce code duplication
# TODO: s3 output should be incremented each time or get from cmd line argument

all: mapper

run: job2

job2: job1
	$(EMR) -j $(JOB_ID) \
		--stream \
		--mapper cat \
		--input $(S3_OUTPUT)/$(S3_OUTDIR) \
		--output $(S3_OUTPUT)/$(S3_OUTDIR)-r2 \
		--reducer $(S3_BIN)/reducer2 \
		--jobconf 'mapred.task.timeout=60000'

job1: bin-up
	$(EMR) -j $(JOB_ID) \
		--stream \
		--mapper $(S3_BIN)/mapper \
		--input $(S3_INPUT) \
		--output $(S3_OUTPUT)/$(S3_OUTDIR) \
		--reducer $(S3_BIN)/reducer \
		--jobconf 'mapred.task.timeout=60000'

start:
	$(EMR) --create --alive --enable-debugging --log-uri $(S3_LOG)

bin-up: mapper reducer reducer2
	s3cmd put $^ $(S3_BIN)/

reducer2: reducer2.cpp
	$(CC) $(CFLAGS) -o $@ $^

reducer: reducer.cpp
	$(CC) $(CFLAGS) -o $@ $^

mapper: mapper.cpp rabinhash64.o
	$(CC) $(CFLAGS) -o $@ $^

rabinhash64.o: third_party/rabinhash-64/rabinhash64.cpp
	$(CC) $(CFLAGS) -c $^
