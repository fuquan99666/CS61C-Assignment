### Tips
- 文档中提供的Camera.jar下载链接需要伯克利学生认证才行，可以使用当前目录下的Camera.jar。
- 需要注意在使用命令行运行VMSIM时，只有java8支持appletviewer命令，所以需要下载一个java8版本的JDK, 由于多个java版本
会导致环境变量混乱，此时直接双击Camera.jar可能无法正常运行，可以直接用命令行运行，命令如下：
```
javaw -jar Camera.jar
// javaw是java的一个版本，区别在于它不会打开命令行，而是运行图形界面程序
// 实际路径需要根据自己的情况修改
// 如果javaw没有添加到系统环境变量，也可以直接使用绝对路径（注意加上双引号）
```