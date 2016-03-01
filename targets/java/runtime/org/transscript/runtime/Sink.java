// Copyright (c) 2014 IBM Corporation.
package org.transscript.runtime;

import org.transscript.runtime.Term.Kind;

/**
 * Consumes Term events.
 * 
 * @author Lionel Villard
 */
public abstract class Sink
{

	/** 
	 * Static helper sending a named or variable property
	 * @param key of the property
	 * @param term property value. The reference is used by this method.
	 * @return continuation sink to use for subsequent operation
	 */
	public static Sink property(Sink sink, Term key, Term value)
	{
		return key.kind() == Kind.VARIABLE_USE ? sink.propertyVariable(((VariableUse) key).variable, value) : sink.propertyNamed(
				key.symbol(), value);
	}

	/**
	 * Start construction.
	 * 
	 * @param descriptor of the construction
	 * 
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink start(ConstructionDescriptor descriptor);

	/**
	 * End of constructor subterm
	 * 
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink end();

	/**
	 * Insert binder wrapper around current construction subterm.
	 * 
	 * @param binders to be bound here - <em>must</em> be fresh (not used in any
	 *            existing terms)
	 * @return continuation sink to use for subsequent operation (never null)
	 */
	public Sink bind(Variable binder)
	{
		return binds(new Variable[]
			{binder});
	}

	/**
	 * Insert binders wrapper around current construction subterm.
	 * 
	 * @param binders to be bound here - <em>must</em> be fresh (not used in any
	 *            existing terms). All variable references are used by this method
	 * @return continuation sink to use for subsequent operation (never null)
	 */
	public abstract Sink binds(Variable[] binders);

	/**
	 * Insert variable occurrence subterm.
	 * 
	 * @param variable to reference. The reference is *NOT* used by this method.
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink use(Variable variable);

	/**
	 * Insert literal subterm.
	 * 
	 * @param literal to add
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink literal(Object literal);

	/**
	 * Insert term.
	 *
	 * <p>
	 * Append properties to it if any. Won't do deep-copy if can be avoided.
	 * 
	 * @param term. The reference is used by this method.
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink copy(Term term);

	/**
	 * Start a meta-application.
	 * @param name of meta-variable to use
	 * @return continuation sink to use for subsequent operation (never null)
	 */
	public abstract Sink startMetaApplication(String name);
	
	/**
	 * End of previously started meta-application subterm.
	 * @return continuation sink to use for subsequent operation
	 *     - may return <b>null</b> if it does not make sense to send further events
	 */
	public abstract Sink endMetaApplication();
	
	/**
	 * Queue properties to be inserted either on a new construction or a term.
	 * 
	 * @param properties to insert. The reference is used by this method
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink properties(Properties properties);

	/**
	 * Queue a named property to be inserted either on a new construction or a
	 * term
	 * 
	 * @param name of the property
	 * @param term property value. The reference is used by this method.
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink propertyNamed(String name, Term term);

	/**
	 * Queue a variable property to be inserted either on a new construction or
	 * a term.
	 * 
	 * @param variable of the property. The reference is used by this method.
	 * @param term property value. The reference is used by this method.
	 * @return continuation sink to use for subsequent operation
	 */
	public abstract Sink propertyVariable(Variable variable, Term term);

	/**
	 * Substitute the binder with the corresponding term.
	 *
	 * <p>
	 * Convenient method for when the term has only one binder.
	 * 
	 * @param term to apply substitution. The reference is used by this method
	 * @param binder the term binder. The reference is *NOT* used by this method
	 * @param substitute. The reference is *NOT* used by this method. 
	 * @return continuation sink to use for subsequent operation
	 */
	public Sink substitute(Term term, Variable binder, Term substitute)
	{
		return substitute(term, new Variable[]
			{binder}, new Term[]
			{substitute});
	}

	/**
	 * Substitute the binders with the corresponding terms.
	 *            
	 * @param term to apply substitution. The reference is used by this method
	 * @param binders the term binder. The references are *NOT* used by this method
	 * @param substitutes. The references are *NOT* used by this method.
	 * @return continuation sink to use for subsequent operation
	 */
	final public Sink substitute(Term term, Variable[] binders, Term[] substitutes)
	{
		return term.substitute(this, binders, substitutes);
	}

	/**
	 * @return the context
	 */
	public abstract Context context();

}