CC = g++
CFLAGS = -Wall -Wextra

S3_BIN = s3://davidhu-mr1/bin
S3_INPUT = s3://davidhu-aquaint
S3_OUTPUT = s3://davidhu-mr1/output
S3_LOG = s3://mapreduce-log-uri
EMR = /home/david/bin/elastic-mapreduce-ruby/elastic-mapreduce

# TODO: create more variables
# TODO: s3 output should be incremented each time or get from cmd line argument

all: mapper

run: mapper-up
	$(EMR) -j $(JOB_ID) \
		--stream \
		--mapper $(S3_BIN)/mapper \
		--input $(S3_INPUT) \
		--output $(S3_OUTPUT)/$(S3_OUTDIR) \
		--reducer aggregate \
		--log-uri $(S3_LOG)

# TODO: Figure out why StreamXmlRecordReader + StreamInputFormat doesn't seem
#     to be working... perhaps try on a local Hadoop install
#--stream \
#--arg '-inputformat' \
#--arg 'StreamInputFormat' \
#--arg '-inputreader' \
#--arg 'StreamXmlRecordReader,begin=<DOC>,end=</DOC>' \
#--mapper $(S3_BIN)/mapper \
#--input $(S3_INPUT) \
#--output $(S3_OUTPUT) \
#--reducer aggregate \
#--log-uri $(S3_LOG)

start:
	$(EMR) --create --alive --enable-debugging --log-uri $(S3_LOG)

mapper-up: mapper
	s3cmd put $< $(S3_BIN)/$<

mapper: mapper.cpp
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: mapper-up
