## README(en)
This program is used to create Chinese crossword puzzles. Unlike other online crossword puzzle generators, this program can automatically use the previous sentence as a clue, greatly improving production efficiency. Therefore, this is more suitable for generating crossword puzzles composed of poetry.
#### Input
Put all the text you want to generate into the data.txt file, ensuring that it is in the same directory as the program. The title of each poem should start with the "%" symbol and do not retain additional information such as author, dynasty, etc. There should be no extra symbols (such as pinyin, comments, etc.) or spaces in the file.
The init.ini file is used to configure initialization information. The first line is a number(unsigned int in C/C++) used as seeds for generating random numbers. The second line is the initial sentence, which serves as the starting point for program generation.
#### Output
The program will generate three files in the current directory, named random number seeds with suffixes of "maps. csv", "clue. txt", and "ans. txt". Among them, "maps. csv" is used to create a printed version of the puzzle. 'clue. txt' is the clue for the puzzle, 'ans.txt' is the answer.

Open maps.csv using Microsoft Excel, press the hotkey Ctrl-G, select location conditions - constants - OK, and draw all the box lines. After adjusting the row width, column width, and font to the appropriate size, it can be printed. Enjoy solving the puzzle!

## About Sun-H454
I'm just a Oier in junior high school.
visit my homepage on Bilibili: [SunH454](https://space.bilibili.com/3546613729462883)

## README(zh-cn)
此程序用于制作中文纵横填字游戏。不同于其他网上已有的填字游戏生成器，此程序可以自动以其上一句作为线索，较大程度上提升了制作效率。因此，这更适合于生成诗词（文）组成的填字游戏。

#### 输入

把所有想要生成的文本放入data.txt文件中，确保其与程序在同一目录下。每一首诗的标题以"%"符号开头，不要保留作者、朝代等附加信息。文件中不要有多余的符号（如拼音、注释等）或空格。

init.ini用来配置初始化信息。第一行是一个正整数（unsigned int）范围，用来作为生成随机数的种子。第二行是初始的句子，作为程序生成的起点。

#### 输出

程序将会在当前目录生成三个文件，分别以随机数种子为名称，后缀分别是"maps.csv"、"clue.txt"、"ans.txt"。其中，"maps.csv"用来制作填字游戏的打印版。"clue.txt"是填字游戏的线索，"ans.txt"是答案。

用Excel软件打开maps.csv，按下热键Ctrl-G，选择定位条件-常量-确定，并绘制所有框线。调整行宽、列宽和字体至合适大小后即可打印。Enjoy solving the puzzle!

## 关于作者
初中生，Oier(只是个蒟蒻)
访问我的 Bilibili 主页: [SunH454](https://space.bilibili.com/3546613729462883)
