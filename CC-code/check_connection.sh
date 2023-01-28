ssh -q cluster@10.20.24.12 exit
echo $? > a.txt
ssh -q cluster@10.20.24.79 exit
echo $? >> a.txt
ssh -q cluster@10.20.24.60 exit
echo $? >> a.txt
