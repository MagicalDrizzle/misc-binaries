Note: Natively compiling in Windows results in output PNGs being corrupted due to line endings conversion. (sources: [1](https://mattscodecave.com/posts/plaidctf-2015---corrupt-png.html), [2](https://fuzyll.com/2015/uncorrupting-a-png-image/))  

Using `dos2unix` fixes it. Therefore, I have included `dos2unix`, `pngcheck` and wrote a little batch wrapper to automates detecting and fixing the PNGs.
