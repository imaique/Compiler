import csv
import re


firsts = []
follows = []
nullables = []

def process(string):
   list = string.split()

with open("firstfollow.csv", 'r') as file:
  csvreader = csv.reader(file)
  columns = next(csvreader)

  regex = re.compile('[^a-zA-Z]')
  for row in csvreader:
      rowhead = row[0]
      string = "{"
      string += f'\"{rowhead}\", '
      string += "{"
      
      firsts.append((rowhead, row[1].split()))
      follows.append((rowhead, row[2].split()))

      if row[3] == "yes":
         nullables.append(rowhead)

  print("first")
  
  for first in firsts:
     string = '{"' + first[0] + '", {'
     list = [regex.sub('', x) for x in first[1]]
     string += '"' + list[0] + '"'
     for el in list[1:]:
        
        string+= ', ' + '"' + el + '"'
     string += '}},'
     print(string)

  print("follow")
  for follow in follows:
     string = '{"' + follow[0] + '", {'
     list = [regex.sub('', x) for x in follow[1]]
     if len(list[0]) == 0: continue
     string += '"' + list[0] + '"'
     for el in list[1:]:
        
        string+= ', ' + '"' + el + '"'
     string += '}},'
     print(string)
  
  
  print("nullable")
  for el in nullables[1:]:
    
    print('"' + el + '",')

