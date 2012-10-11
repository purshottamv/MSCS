#! /bin/bash
java SequentialPageRank pagerank.input.1K0 pagerank.output 10 0.85 >> outputseq
echo Finished 1K0
java SequentialPageRank pagerank.input.10K0 pagerank.output 10 0.85 >> outputseq
echo Finished 10K0
java SequentialPageRank pagerank.input.20K0 pagerank.output 10 0.85 >> outputseq
echo Finished 20K0
java SequentialPageRank pagerank.input.30K0 pagerank.output 10 0.85 >> outputseq
echo Finished 30K0
java SequentialPageRank pagerank.input.40K0 pagerank.output 10 0.85 >> outputseq
echo Finished 40K0
java SequentialPageRank pagerank.input.50K0 pagerank.output 10 0.85 >> outputseq
echo Finished 50K0
java SequentialPageRank pagerank.input.60K0 pagerank.output 10 0.85 >> outputseq
echo Finished 60K0
java SequentialPageRank pagerank.input.70K0 pagerank.output 10 0.85 >> outputseq
echo Finished 70K0
java SequentialPageRank pagerank.input.80K0 pagerank.output 10 0.85 >> outputseq
echo Finished 80K0
java SequentialPageRank pagerank.input.90K0 pagerank.output 10 0.85 >> outputseq
echo Finished 90K0
java SequentialPageRank pagerank.input.100K0 pagerank.output 10 0.85 >> outputseq
echo Finished 100K0
java SequentialPageRank pagerank.input.500K0 pagerank.output 10 0.85 >> outputseq
echo Finished 500K0
java SequentialPageRank pagerank.input.1M pagerank.output 10 0.85 >> outputseq
echo Finished 1M
java SequentialPageRank pagerank.input.2M pagerank.output 10 0.85 >> outputseq
echo Finished 2M
