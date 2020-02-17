#!/usr/local/bin/perl -w

use Net::SMTP;
use POSIX;

$smtp = Net::SMTP->new('gw.home.your.org', Port => '25', Timeout => '1');

$smtp->mail('localhost');
$smtp->to('localhost');
$smtp->data();
$smtp->datasend("Date: " . strftime("%a, %d %b %Y %H:%M:%S %z", localtime) . "\r\n");
$smtp->datasend("Content-Type: text/html; charset=gsm-7\r\n");
$smtp->datasend("Content-Transfer-Encoding: base64\r\n");
$smtp->datasend("Content-Disposition: inline\r\n");
while (<>) {
    $smtp->datasend($_);
}
$smtp->quit;

exit 0

