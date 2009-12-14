print("111")

os.execute("echo 10 > /root/progress.txt")
os.execute("sleep 10")
os.execute("echo 20 > /root/progress.txt")
os.execute("sleep 10")
os.execute("echo 50 > /root/progress.txt")
os.execute("sleep 10")
os.execute("echo 70 > /root/progress.txt")

printMsg("hahaha")

return 0
