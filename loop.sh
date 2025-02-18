i=1
while [ $i -le 100 ]; do
  echo "This is iteration number $i"
  i=$((i + 1))
  sleep 1
done