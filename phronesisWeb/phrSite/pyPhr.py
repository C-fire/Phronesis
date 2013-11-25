import pyPhronesis as phr
import threading
import time
import thread

class pyApi(phr.AbstractPhronesisClient):
  fileName = "/home/chaen/django/phronesis/phronesis/phrSite/static/scripts/demo_test.txt"
  content = []
  def update(self, msg):
    print "I am called"
    print msg.m_text
    self.prepend(msg.m_text)

  def prepend(self, msg):
    print "content before : %s"%self.content
    self.content.reverse()
    self.content.append(msg)
    self.content.reverse()
    print "content after : %s"%self.content
    self.writeInFile()
 
  def writeInFile(self):
    f = open(self.fileName, 'w')
    for l in self.content:
        f.write(l)
    f.close()

class otherClass:
  def doWhatever(self):
    for i in range(0,10):
      print "I do whatever ",i
      time.sleep(1)        

   



p = pyApi()
#for i in range(0, 10):
#    m = phr.api_message(0,1,"%s"%i*i)
#    p.update(m)
time.sleep(1)
#p.run("localhost","7172")
#
print "Before"
thr = threading.Thread(None, p.run, None, ("localhost", "7172",),{})
#thread.start_new_thread(p.run, ("localhost", "7172",))
o = otherClass()
thread.start_new_thread(o.doWhatever, ())
time.sleep(1)
thr.start()
print "after. Wait a bit..."
time.sleep(5)
print "I will stop it now"
p.quit()
print "I did it :-) "
#
#thr.stop()
