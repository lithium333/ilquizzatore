questions=list()
vector=[]

with open("pool.txt", "r") as file:
	question=[]
	for line in file:
		if line=='\n' and question!=[]:
			questions.append(question)
			question=[]
		else:
			for char in line:
				while char!=chr(127):
					vector.append(char)
					


for question in questions:
	nans = question[0]
	ismedia = question[1]
	testo = question[2]
	risposte = question[3:int(nans)+3]

	print(f"{nans} {ismedia} {risposte}")







