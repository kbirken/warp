
@echo off
set dot="C:\Program Files\Graphviz 2.28\bin\dot.exe"

rem generate simulation graph
rem %dot% -Tgif -o trace_dot.gif trace_dot.txt
%dot% -Tpdf -o trace_dot.pdf trace_dot.txt
