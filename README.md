# RootExamples
Some simple ROOT macros useful for data analysis and simulation

## Excel to ROOT Analysis
The first three files (excelplot1, excelplot2 and excelplot3) are useful when data is taken on excel (or libreoffice) and then the analysis is performed on ROOT. 

### Saving Data
- First of all, ROOT needs decimal point separators instead of decimal comma. In Italy, Excel and Libreoffice use the latter.
  To solve this problem, go to Excel (or Libreoffice).
  For LibreOffice -> Go to Instruments -> Options -> Language Options -> Select English(UK) or US
  For Excel -> Go to File -> Options -> Advanced -> Uncheck "Use System Separator" -> Type point
- Then, to save a file in Excel (or Libreoffice)
  For LibreOffice -> SaveAs(".csv") -> Then choose "Blank Space" in "Field Separator" -> Write nothing on String Separator and Save
  For Excel -> Go to Control Panel -> Regional Settings -> Additional Settings -> Change Field Separator
  
### Plotting
- excelplot1
  It reads only two columns of data (xdata + blank space +ydata). Then, if you wish to add uncertainties you need to do it inside the macro. It fills and draws a TGraphErrors.
- excelplot2
  It reads four columns of data (xdata + blank space + ydata + blank space + exdata + blank space + eydata). This means that uncertainties have already been computed in Excel.
- excelplot3
  It reads only one column of data and fills an histogram
- excelplot4
  It reads only two columns of data without uncertainties and fills a TGraph.
