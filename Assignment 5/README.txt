I did not prevent enc_client from connecting to dec_server or vice versa. (minus 5 pts)
I did not chunk the data, so plaintext4 partially fails in that some of the data is lost due to being too large.
      Plaintext1, 2, 3, and 5 are all correctly handled.

To run:

-- compileall
-- if compileall doesnt work: 'dos2unix compileall', try again
-- ./p5testscript RANDOM_PORT1 RANDOM_PORT2 > mytestresults 2>&1
-- if permission denied on test script: 'chmod u+x p5testscript', try again