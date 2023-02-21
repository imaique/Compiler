import csv

def remove_arrow(string):
   index = string.find('?')
   return string[index+2:]

rows = []
rules_list = []
rules = {}
with open("data.csv", 'r') as file:
  csvreader = csv.reader(file)
  columns = next(csvreader)
  for column in columns:
     print(f'cs t_{column} = \"{column}\";')
     
  for row in csvreader:
      rowhead = row[0]
      string = "{"
      string += f'\"{rowhead}\", '
      string += "{"
      for index, value in enumerate(row[1:], 1):
         
         if value == "":
            continue
         element_str = "{"
         element_str += f't_{columns[index]}, '
         rule = ""
         arrowless = remove_arrow(value)

         if arrowless in rules:
            rule = rules[arrowless]
         else:
            rule = f'r{len(rules) + 1}'
            rules[arrowless] = rule
            rules_list.append(arrowless)

         element_str += rule + '},'
         string += element_str
      string += "}},"
      print(string)
  
  for index, rule in enumerate(rules_list, 1):
     print(f'cs r{index} = \"{rule}\";')

         


        


