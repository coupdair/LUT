/*Additional documentation for the generation of the reference page (using doxygen)*/
/**
\mainpage RockAMali software suite
*
* \image html logo.png
*
* table of content:
* \li \ref sectionCodeHistory
* \li \ref sectionCommandLine
* \li \ref sectionServerCommand
* \li \ref sectionDriverCommand
* \li \ref sectionSCserverDocumentation
* \li \ref sectionDoxygenSyntax
*
* \section sectionCodeHistory Modifications
*

    S. Coudert 26/08/2019
     - doxygen for entire software suite

* \section sectionCommandLine command line options
*
* - all process that have processing class might have \c --use-GPU CLI option if \c DO_GPU is set in \c Makefile
*
* - process
* \verbinclude "process_help.output"
*
* - process (sequential version)
* \verbinclude "process_sequential_help.output"
*
* - receive (see especially \c -c and \c --do-check CLI options)
* \verbinclude "receive_help.output"
*
* - send
* \verbinclude "send_help.output"
*
* - store (old process)
* \verbinclude "store_help.output"
*
* \section sectionSCserverDocumentation documentation outline
* This is the reference documentation of 
* <a href="http://wiki.ganil.fr/gap/">NUMEXO2 register server</a>, 
* from the <a href="http://www.ganil-spiral2.eu">GANIL</a>.\n\n
* The main function is in <a href="SC__server__numexo_8cc.html">SC_server_numexo.cc</a> source file.\n\n
* This documentation has been automatically generated from the sources,
* using the tool <a href="http://www.doxygen.org">doxygen</a>. It should be readed as HTML, LaTex and man page.\n
* It contains both
* \li a detailed description of all classes and functions
* \li TODO: a user guide (cf. \ref pages.html "related pages")
*
* that as been documented within the sources.
*
* \par Additional needed libraries:
*
* \li \c gc_ref:   see \c gc_liste_string.h.
* \li \c FDT:      see \c fdt.h
* \li \c Drivers:  see \c Device.hxx
*
* \section sectionDoxygenSyntax make documentation using Doxygen syntax
* Each function in the source code should be commented using \b doxygen \b syntax in the same file.
* The documentation need to be written before the function.
* The basic syntax is presented in this part.
* \verbinclude "doxygen.example1.txt"
*
* Two kind of comments are needed for \b declaration and \b explanation \b parts of the function:
* Standart documentation should the following (\b sample of code documentation):
* \verbinclude "doxygen.example2.txt"
*
* In both declaration and explanation part, \b writting and \b highlithing syntax can be the following:\n\n
*
* \li \c \\n a new line
* \li \c \\li a list (dot list)
*
* \li \c \\b bold style
* \li \c \\c code style
* \li \c \\e enhanced style (italic)
*
* For making \b shortcut please use:\n
* \li \c \\see to make a shortcut to a related function or variable
* \li \c \\link to make a shortcut to a file or a function
* \note this keyword needs to be closed using \c \\end*
*
* \li \c \\todo to add a thing to do in the list of <a href="todo.html">ToDo</a> for the whole program
*
* In explanation part, \b paragraph style can be the following:\n
* \li \c \\code for an example of the function use
* \li \c \\note to add a few notes
* \li \c \\attention for SOMETHING NOT FULLY DEFINED YET
* \li \c \\warning to give a few warning on the function
* \note these keywords need to be closed using \c \\end*
*
* \verbinclude "doxygen.example3.txt"
*
* Many other keywords are defined, so please read the documentation of <a href="http://www.doxygen.org/commands.html">doxygen</a>.
*
**/

/**
\page pageServerCommand NUMEXO2 register server commands
* table of content:
* \li \ref sectionCmdB3
* \li \ref sectionCmdC2SA
* \li \ref sectionCmdFADC
* \li \ref sectionCmdGTS
* \li \ref sectionCmdV5
* \li \ref sectionCmdV6
* \li \ref sectionCmdV6_S3
*
* \section sectionCmdB3 B3
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.B3.output"
*
* \section sectionCmdC2SA C2SA
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.C2SA.output"
*
* \section sectionCmdFADC FADC
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.FADC.output"
*
* \section sectionCmdGTS GTS
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.GTS.output"
*
* \section sectionCmdV5 V5
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.V5.output"
*
* \section sectionCmdV6 V6
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.V6.output"
*
* \section sectionCmdV6_S3 V6 S3
* \verbinclude "/home/coudert/SC_Embedded/trunk/NUMEXO2/SCserver.service.V6_S3.output"
*
**/

/**
\page pageGit Git repository for NUMEXO2 register server
* table of content:
* \li \ref sectionGitLayout
*
* \section sectionGitLayout Git repository layout (and history)
*
* Git repository
*
* \image html git_graph.png "git layout"
*
* \li branches in blue  color
* \li projects in green color
*
**/

/**
\page pageConnection server connections
* device and client relations with SC server:
* \dot
* digraph connection {
* node[shape=record];
**
* //nodes
* ///device nodes
* sensor[ label="temperature registers\nmax6627 as sensorV5nV6" URL="\ref module_numexo2.hxx"];
* v6[ label="Virtex6 registers" URL="\ref device_spi_v6_numexo.cc"];
* b3[ label="B3 registers" URL="\ref device_spi_b3_numexo.cc"];
* v5[ label="Virtex5/PLL registers" URL="\ref device_v5_numexo.cc"];
* gts[ label="GTS registers" URL="\ref device_numexo2_gts.cc"];
* adc[ label="ADC registers" URL="\ref device_adc_interface_numexo.cc"];
* frame[ label="frame readout" URL="\ref device_numexo2_readout.cc"];
* ///driver nodes
* frame_driver[  label="Virtex5 frame readout\n/dev/my_fifo driver\n(custom)" shape=ellipse];
* memory_driver[ label="register by memory map\n/dev/memory driver\n(custom)" shape=ellipse URL="\ref device_memory"];
* spi_driver[    label="register by SPI\n/dev/spidev* driver\n(standard)"     shape=ellipse URL="\ref spi"];
* //[ label="" URL="\ref "];
* server[ label="Virtex5/PPC\nthis SC server" URL="\ref SC_server_numexo.cc" color=green];
*
* //links
* spi_driver->sensor[style="dashed"];sensor->server[label="SPI"];
* spi_driver->v6[style="dashed"];v6->server[label="SPI"];
* spi_driver->b3[style="dashed"];b3->server[label="SPI"];
* spi_driver->v5[style="dashed"];
* memory_driver->v5[style="dashed"];v5->server[label="mmap or SPI"];
* memory_driver->gts[style="dashed"];gts->server[label="mmap"];
* memory_driver->adc[style="dashed"];adc->server[label="mmap"];
* frame_driver->frame[style="dashed"];frame->server[label="readout"];
**
* //client nodes
* SC_client[label="SC client" color=blue];
* GECO[label="GECo" color=blue];
* NARVAL[label="NARVAL" color=blue];
*
* //links
* server->SC_client[label="SOAP"];SC_client->server
* server->GECO[label="SOAP"];GECO->server
* server->NARVAL[label="TCP"];
**
* }
* \enddot
* Note: a few nodes of the above graph are clickable and link to corresponding code
* (in the HTML output), e.g. devices.
**/

