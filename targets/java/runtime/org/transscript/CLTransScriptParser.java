// Copyright (c) 2015 IBM Corporation.

package org.transscript;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import org.antlr.v4.runtime.ANTLRInputStream;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.DiagnosticErrorListener;
import org.antlr.v4.runtime.TokenSource;
import org.antlr.v4.runtime.TokenStream;
import org.transscript.antlr.SinkAntlrListener;
import org.transscript.parser.TransScriptTermLexer;
import org.transscript.parser.TransScriptTermParser;

import net.sf.crsx.Variable;
import net.sf.crsx.generic.GenericFactory;
import net.sf.crsx.generic.GenericTerm;
import net.sf.crsx.util.Buffer;
import net.sf.crsx.util.FormattingAppendable;

/**
 * Simple utility to invoke the CRSX parser from the command line.
 * 
 * @author villardl
 */
public class CLTransScriptParser
{

	public static void printUsage()
	{
		System.out.println("java org.crsx.parser.CLCrsxParser input.crs output.term");
		System.exit(-1);
	}

	public static void main(String[] args)
	{
		if (args.length != 2)
			printUsage();

		String inputname = args[0];
		String outputname = args[1];
		try
		{
			CharStream stream = new ANTLRInputStream(new FileReader(inputname));

			TokenSource source = new TransScriptTermLexer(stream);
			TokenStream input = new CommonTokenStream(source);

			TransScriptTermParser parser = new TransScriptTermParser(input);
			parser.setBuildParseTree(false);
			//parser.setTrace(true);

			GenericFactory factory = new GenericFactory();
			Buffer buffer = new Buffer(factory);
			SinkAntlrListener listener = new SinkAntlrListener(factory, buffer.sink(), "TransScript_", "##", parser, null);
			parser.addParseListener(listener);
			parser.addErrorListener(new DiagnosticErrorListener(true));
			parser.transscript();

			GenericTerm term = (GenericTerm) buffer.term(true);

			File outputFile = new File(outputname);
			outputFile.getParentFile().mkdirs();
			FileWriter w = new FileWriter(outputFile);
			FormattingAppendable f = FormattingAppendable.format(w, 120, 0, Integer.MAX_VALUE);
			Map<Variable, String> variableNames = new HashMap<Variable, String>();
			term.appendTermTo(
					f, variableNames, factory.defined(GenericFactory.NO_LINEAR_VARIABLES), Integer.MAX_VALUE, false, false, false,
					false, new HashSet<Variable>(), false);

			w.close();
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}

}
