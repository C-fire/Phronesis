import MySQLdb, sys

url = 'localhost'
user = 'root'
passwd = ''
dbName = 'phronesisAnalyzer'
port = 3306
req = "select id, name, id_total, valid from MetaAgent;"
try:
    database = MySQLdb.connect(url, user, passwd, dbName, port)
except Exception, err:
    print "Connection to the database failed : " + str(err)
    sys.exit(1)

cursor = database.cursor()   # creation du curseur
try:
    cursor.execute(req)
    result = cursor.fetchall()
    print result
except Exception, err:
            #The SQL request has an error
            print "Error in the SQL request : " + req + " " + str(err)
            sys.exit(1)
database.close()



