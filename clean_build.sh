verbose=
serial=-j
while getopts "vsh" opt; do
	case $opt in
		v)
			verbose="VERBOSE=1"
			echo "printing make with details"
			;;
		s)
			serial=
			echo "making serially"
			;;
		h)
			echo "Usage of $(basename "$0"):"
			echo "	to clean the bin directory then builds the code and run it "
			echo ""
			echo "-v	: to print the output of make in details"
			echo ""
			echo "-s	: to compile serially rather than in parallel"
			echo ""
			exit 1
			;;
	esac
done

cd bin/
make clean
make acoustic_engine acoustic_modeller $serial $verbose
cd ../
