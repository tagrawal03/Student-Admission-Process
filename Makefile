all :
	g++ -o Admissionoutput Admission.cpp -lnsl -lsocket -lresolv 
	g++ -o DepartmentAoutput DepartmentA.cpp -lnsl -lsocket -lresolv 
	g++ -o DepartmentBoutput DepartmentB.cpp -lnsl -lsocket -lresolv 
	g++ -o Student1output Student1.cpp -lnsl -lsocket -lresolv 
	g++ -o Student2output Student2.cpp -lnsl -lsocket -lresolv 
	g++ -o Student3output Student3.cpp -lnsl -lsocket -lresolv 
	g++ -o Student4output Student4.cpp -lnsl -lsocket -lresolv 
	g++ -o Student5output Student5.cpp -lnsl -lsocket -lresolv 

clean :
	rm -rf DepA.txt DepB.txt DepartmentA DepartmentB Admission Stu1.txt Stu2.txt Stu3.txt Stu4.txt Stu5.txt Student1 Student2 Student3 Student4 Student5 *_result.txt
