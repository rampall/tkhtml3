
catch { memory init on }

# Load packages.
if {[info exists auto_path]} {
    set auto_path [concat . $auto_path]
}
package require Tk
package require Tkhtml 3.0

# If possible, load package "Img". Without it the script can still run,
# but won't be able to load many image formats.
if {[catch { package require Img }]} {
  puts "WARNING: Failed to load package Img"
}

# Source the other script files that are part of this application.
#
proc sourcefile {file} {
  set fname [file join [file dirname [info script]] $file] 
  uplevel #0 [list source $fname]
}

sourcefile hv3_log.tcl
sourcefile hv3_prop.tcl
sourcefile hv3_form.tcl
sourcefile hv3.tcl

#--------------------------------------------------------------------------
# gui_build --
#
#     This procedure is called once at the start of the script to build
#     the GUI used by the application. It assumes a hv3 object ".hv3"
#     has already been created, but not [pack]ed.
#
proc gui_build {} {
  global HTML

  frame .entry
  entry .entry.entry
  button .entry.clear -text {Clear ->} -command {.entry.entry delete 0 end}

  pack .entry.clear -side left
  pack .entry.entry -fill both -expand true
  pack .entry -fill x -side top 
  bind .entry.entry <KeyPress-Return> {hv3Goto .hv3 [.entry.entry get]}

  pack .hv3 -fill both -expand true
  focus .hv3.html

  # Build the main window menu.
  . config -menu [menu .m]
  .m add cascade -label {File} -menu [menu .m.file]
  .m.file add command -label Back -command guiBack -state disabled
  .m.file add separator
  foreach f [list \
    [file join $::tcl_library .. .. bin tkcon] \
    [file join $::tcl_library .. .. bin tkcon.tcl]
  ] {
    if {[file exists $f]} {
      catch {
        uplevel #0 "source $f"
        package require tkcon
        .m.file add command -label Tkcon -command {tkcon show}
      }
      break
    }
  }
  .m.file add command -label Browser -command [list prop_browse $HTML]
  .m.file add separator
  .m.file add command -label Exit -command hv3_exit

  # Add the 'Font Size Table' menu
  .m add cascade -label {Font Size Table} -menu [menu .m.font]
  foreach {label table} [list \
    Normal {7 8 9 10 12 14 16} \
    Large  {9 10 11 12 14 16 18} \
    {Very Large}  {11 12 13 14 16 18 20} \
    {Extra Large}  {13 14 15 16 18 20 22} \
    {Recklessly Large}  {15 16 17 18 20 22 24}
  ] {
    .m.font add command -label $label -command [list \
      $HTML configure -fonttable $table
    ]
  }
}

#--------------------------------------------------------------------------
# Implementation of File->Back command.
#
#     Proc guiGotoCallback is registered as the -gotocallback with hv3
#     object .hv3. It maintains a URL history for the browser session in
#     global variable ::hv3_history_list
#    
#     Proc guiBack is invoked when the "Back" command is issued.

proc guiGotoCallback {url} {
  lappend ::hv3_history_list $url
  .entry.entry delete 0 end
  .entry.entry insert 0 $url
  if {[llength $::hv3_history_list]>1} {
    .m.file entryconfigure Back -state normal
  }
}

proc guiBack {} {
  if {[llength $::hv3_history_list]>1} {
    set newurl [lindex $::hv3_history_list end-1]
    set ::hv3_history_list [lrange $::hv3_history_list 0 end-2]
    hv3Goto .hv3 $newurl
  }
  if {[llength $::hv3_history_list]<=1} {
    .m.file entryconfigure Back -state disabled
  }
}

# End of implementation of File->Back command.
#--------------------------------------------------------------------------


#--------------------------------------------------------------------------
# Implementation of http:// protocol for hv3.
#
#     We require an http proxy running on the localhost, tcp port 3128, '
#     for this to work. This command depends on the tcl http package.
#
#     One global variable is used:
#    
#         ::hv3_http_token_list
#
#     This stores the list of outstanding http tokens. It is used to
#     cancel downloads if [httpProtocol -reset] is called.
#
#     Two procs are declared:
#
#         httpProtocol
#         httpProtocolCallback
# 

# httpProtocol
#
#     This command is registered as the handler for the http:// protocol.
#
swproc httpProtocol {url {script ""} {binary 0}} {

  # If this is the first invocation of this proc, initialise
  # the global outstanding-tokens list to an empty list. Also
  # configure the http module to use the web proxy.
  if {![info exists ::hv3_http_token_list]} {
    package require http
    set ::hv3_http_token_list [list]
    ::http::config -proxyhost localhost
    ::http::config -proxyport 3128
  }

  # If the caller has invoked [httpProtocol -reset] cancel all 
  # currently outstanding downloads and return.
  if {$url=="-reset"} {
    foreach token $::hv3_http_token_list {
      ::http::cleanup $token
    }
    return
  }

  puts $url

  # Start the download and append the token to the global token
  # list. When the download is finished, [httpProtocolCallback] will
  # be invoked.
  set cmd [list httpProtocolCallback $script $binary]
  set token [::http::geturl $url -command $cmd]
  lappend ::hv3_http_token_list $token
}

# httpProtocolCallback 
#
#     This proc is invoked when an http request made by proc httpProtocol
#     has finished downloading.
#
proc httpProtocolCallback {script binary token} {
  set cmd $script
  lappend cmd [::http::data $token]
  eval $cmd
}

# End of http:// protocol implementation
#--------------------------------------------------------------------------

#
# Override the [exit] command to check if the widget code leaked memory
# or not before exiting.
#
rename exit tcl_exit
proc exit {args} {
  global HTML
  destroy $HTML 
  catch {destroy .prop$HTML}
  catch {::tk::htmlalloc}
  if {[llength [form_widget_list]] > 0} {
    puts "Leaked widgets: [form_widget_list]"
  }
  eval [concat tcl_exit $args]
}

# main URL
#
proc main {{doc index.html}} {
  hv3Init .hv3 -gotocallback guiGotoCallback
  hv3RegisterProtocol .hv3 http httpProtocol
  set ::HTML .hv3.html
  gui_build
  log_init .hv3.html
  form_init .hv3.html
  hv3Goto .hv3 $doc
}
eval [concat main $argv]
