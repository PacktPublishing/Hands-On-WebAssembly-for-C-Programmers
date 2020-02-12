
// Taken from RosettaCode QuickSort : 
// https://rosettacode.org/wiki/Sorting_algorithms/Quicksort#JavaScript
// Licensed from https://www.gnu.org/licenses/old-licenses/fdl-1.2.html

//removed less as an argument
function sort(array) {
 
  function swap(i, j) {
    var t = array[i];
    array[i] = array[j];
    array[j] = t;
  }
 
  function quicksort(left, right) {
 
    if (left < right) {
      var pivot = array[left + Math.floor((right - left) / 2)],
          left_new = left,
          right_new = right;
 
      do {
	      //changed less() to <
        while (array[left_new] < pivot) {
          left_new += 1;
        }
	      //changed less() to <
        while (pivot < array[right_new]) {
          right_new -= 1;
        }
        if (left_new <= right_new) {
          swap(left_new, right_new);
          left_new += 1;
          right_new -= 1;
        }
      } while (left_new <= right_new);
 
      quicksort(left, right_new);
      quicksort(left_new, right);
 
    }
  }
  quicksort(0, array.length - 1);
  return array;
}

times = []
for (i = 0; i < 1000; ++i) {
	nums = Array.from({length: 100000}, () => Math.floor(Math.random() * 100000));
	now = Date.now();
	sort(nums, 0, nums.length - 1);
	later = Date.now();
	times.push(later - now);
}
console.log(times.reduce((a,b) => a+b) / times.length)
