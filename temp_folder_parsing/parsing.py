questions=list()
vector=[]

with open("pool.txt", "r") as file:
	question=[]
	counter=0
	for line in file:
		if line=='\n' and question!=[]:
			questions.append(question)
			question=[]
		elif counter<2:
			question.append(line.rstrip("\n"))
			counter+=1
		else:
			for char in line:
				if char!=chr(127):
					vector.append(char)
			if vector!=[]:
				question.append("".join(vector))
				vector=[]

					
				


for question in questions:
	nans = question[0]
	ismedia = question[1]
	correct = question[int(nans)+3]
	testo = question[2]
	risposte = question[3:int(nans)+3]

	print(f"{nans} {ismedia} {correct} {testo} {risposte}")

	






