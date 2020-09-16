echo "start $1"
cd ../build/experiments/
rm -rf $3
./cell_move_router $1 $2 >> $3
echo "done $1"

