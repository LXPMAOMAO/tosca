// Copyright (c) 2015 IBM Corporation.
package org.crsx.gradle

import org.gradle.api.DefaultTask
import org.gradle.api.tasks.TaskAction
import org.gradle.api.tasks.*

class PG4Task extends DefaultTask {
	
	@InputFile
	File source 
	
	@OutputDirectory
	File generatedFileDir = project.file("${project.buildDir}/pg4")
	
	@Input
	@Optional
	boolean sort = true
	
	@Input
	@Optional
	boolean parsers = true
	
	@Input
	@Optional
	String metaPrefix = ""
	
	@TaskAction
	def generate() {
		if (sort || parsers)
		{
			// Configure PG runner
			MainRunner pgrunner = new MainRunner(project.configurations.crsx4.files, "org.crsx.antlr.PG")
			
			// Configure Crsx3 runner
			MainRunner crsx3runner = new MainRunner(project.configurations.crsx4.files, "net.sf.crsx.run.Crsx")
				
			// Generate grammars.
			String absname = source.absolutePath
			String nameext = source.name
			String name =  nameext.lastIndexOf('.').with {it != -1 ? nameext[0..<it] : nameext}
			String basename = generatedFileDir.absolutePath + "/" + name
			
			// .g4 -> .term
			String term = absname + '.term'   // grammar as term file
			pgrunner.run([ absname, term ])
			
			// .term -> .nterm
			String nterm = absname + '.nterm' // normalize grammar
			crsx3runner.run([ 'grammar=(\'net.sf.crsx.text.Text\';\'org.crsx.antlr.ANTLRMeta\';)', 'rules=pg/normalizer.crs', 'input=' + term, 'wrapper=Normalize', 'output=' + nterm ])
			
			if (sort)
			{	
				// .nterm -> sort
				
				String sortt = basename + '.crs' // generate sort
				crsx3runner.run([ 'sink=net.sf.crsx.text.TextSink', 'grammar=(\'net.sf.crsx.text.Text\';\'org.crsx.antlr.ANTLRMeta\';)', 'rules=pg/gensort.crs', 'input=' + nterm, 'wrapper=MakeSorts', 'output=' + sortt ])
				
				String sortt4 = basename + '.crs4' // generate sort for crsx4
                crsx3runner.run([ 'sink=net.sf.crsx.text.TextSink', 'grammar=(\'net.sf.crsx.text.Text\';\'org.crsx.antlr.ANTLRMeta\';)', 'rules=pg/gensort.crs', 'input=' + nterm, 'wrapper=MakeSorts', 'crsx4', 'output=' + sortt4 ])
			}
			
			if (parsers)
			{
				def commonargs = []
				commonargs << 'sink=net.sf.crsx.text.TextSink'
				commonargs << 'grammar=(\'net.sf.crsx.text.Text\';\'org.crsx.antlr.ANTLRMeta\';)'
			
				// .nterm -> term lexer/parser
				String termparser = basename + 'Term.g4'
				crsx3runner.run([ 'sink=net.sf.crsx.text.TextSink', 'grammar=(\'net.sf.crsx.text.Text\';\'org.crsx.antlr.ANTLRMeta\';)', 'rules=pg/genparser.crs', 'input=' + nterm, 'wrapper=MakeParser', 'output=' + termparser ])
					
				// .nterm -> meta lexer
				String metalexer = basename + 'MetaLexer.g4' // generate meta lexer
				def args = commonargs.collect()
				args << 'rules=pg/genparser.crs'
				args << "input=${nterm}"
				args << 'wrapper=MakeMetaLexer'
				args << "output=${metalexer}"
				if (!"".equals(metaPrefix))
					args << "metaprefix=${metaPrefix}"
					
				crsx3runner.run(args)
				
				// .nterm -> meta parser
				String metaparser = basename + 'MetaParser.g4' // generate meta parser
				args = commonargs.collect()
				args << 'rules=pg/genparser.crs'
				args << "input=${nterm}"
				args << 'wrapper=MakeMetaParser'
				args << "output=${metaparser}"
				if (!"".equals(metaPrefix))
					args << "metaprefix=${metaPrefix}"
					
				crsx3runner.run(args)
			}
			// Cleanup
			project.delete(term)
			project.delete(nterm)
		}	
	}

}