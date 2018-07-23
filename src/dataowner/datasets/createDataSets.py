#coding:utf-8


'''
Given parameters outputfile name 'inputdata', *, ?, and  % ,this program outputs two files, "inputdataA*_%" and "inputdataB?_%", which include lists of data (string).
For example, if the parameters are 100, 50, 1, it outputs inputdataA10_1 and inputdataB5_1
inputdataA100_1 says that there is 100 words in a set A and one word among them is in the intersection of set A and set B.

To run this program, you need to input name of output file, the size of set A, set B, and the intersection in this order. 

input example:./FILENAME, inputdata 100 50 1

output example:
inputdataA100_1.txt--------------
100
hello
table
.
.
.
water
---------------------------------
/inputdataB50_1.txt--------------
50
sample
pen
.
.
.
water
---------------------------------
'''


import random
import sys

#ファイルの先頭num個のwordを含むlistを生成
def readWordList(filename,num):
        file=open(filename,"r")
        wordList=[]
        for i in range(num):
                string=file.readline()
                wordList.append(string)
        file.close()
        return wordList






def writeWordList(filename,wordList):
        file=open(filename,"w")
        num=len(wordList)
        file.write(str(num)+"\n")
        for i in range(num):
                file.write(wordList[i])
        file.close()


def generateWordList(outputfilename,totalSizeA, totalSizeB,shareSize):
        listA=readWordList("wordA.txt",totalSizeA-shareSize);
        listB=readWordList("wordB.txt",totalSizeB-shareSize);
        sharelist=readWordList("shareWord.txt",shareSize);

        listA.extend(sharelist)
        listB.extend(sharelist)

        #randomize the lists
        random.shuffle(listA);
        random.shuffle(listB);

        outputfilenameA=outputfilename+"A"+str(totalSizeA)+"_"+str(shareSize)+".txt"
        outputfilenameB=outputfilename+"B"+str(totalSizeB)+"_"+str(shareSize)+".txt"

        writeWordList("./datasetsA/"+outputfilenameA,listA)
        writeWordList("./datasetsB/"+outputfilenameB,listB)

argvs=sys.argv
outputfilename=argvs[1]
totalSizeA=int(argvs[2])
totalSizeB=int(argvs[3])
shareSize=int(argvs[4])

generateWordList(outputfilename,totalSizeA,totalSizeB,shareSize)

